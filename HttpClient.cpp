
//
// Created by ak on 12.2.2026.
//

#include <stdio.h>
#include <string.h>

#include "lwip/dns.h"               // Hostname resolution
#include "lwip/altcp_tls.h"         // TCP + TLS (+ HTTP == HTTPS)
#include "altcp_tls_mbedtls_structs.h"
#include "lwip/prot/iana.h"         // HTTPS port number

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/tcp.h"
#include "lwip/dns.h"
#include  "HttpClient.h"
#include "certificate.h"
#include <time.h>

#include "pico/aon_timer.h"
#include "pico/util/datetime.h"
#include "mbedtls/debug.h"


bool wifi_connected_helper()
{
    int status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

    return status == CYW43_LINK_UP;
}

void disconnect_wifi_helper()
{
    printf("Leaving current WiFi\n");
    cyw43_arch_lwip_begin(); //lwip lock
    cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA); //leave current wifi
    cyw43_arch_lwip_end(); //lwip unlock

}

bool connect_wifi_helper(const char* wifi_ssid, const char* wifi_password)
{
    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return false;
    }
    cyw43_arch_enable_sta_mode(); //enable wifi

    printf("Connecting to WiFi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_password,CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT))
    {
        printf("WiFi failed\n");
        return false;
    }else{
        printf("tassaa");
        return  true;
    }
}

HttpClient::HttpClient() //constructor
{
    //init variables
    server_connect_status = false;
    ready_ = false;
    buffer_index_ = 0;
    request_fail_ = false;
    request_fail_ = false;
    tls_config_ = nullptr;
    wifi_initialized_ = false;

    request_path_[0] = '\0';
    request_method_[0] = '\0';
    request_body_[0] = '\0';
    content_type_[0] = '\0';
    ca_cert_initialized_ = false;


    for (int i = 0; i < MAX_HEADERS; i++) {
        request_headers_[i].key[0] = '\0';
        request_headers_[i].value[0] = '\0';
    }

}

void HttpClient::dns_cb(const char *name, const ip_addr_t *ipaddr, void *arg)
{
    HttpClient* client = static_cast<HttpClient*>(arg);
    if (!ipaddr) {
        printf("DNS failed\n");
        client->set_connection_status(false);
    }else{
        client->set_connection_status(true);
        client->set_server_ip_address(*ipaddr);
    }
    client->ready_ = true;
}


void HttpClient::connect_to_server(const char* server_address)
{
    server_host_name_ = server_address;

    ip_addr_t test;
    dns_gethostbyname(server_host_name_, &test, dns_cb, (void*)this);
}

void HttpClient::set_ca_cert(const char* cert, size_t length) { //https
    if(length < MAX_CERTIFICATE_LEN) {
        memcpy(ca_cert_, cert, length);
        cert_length_ = length;
        ca_cert_initialized_ = true;
        return;
    }
    ca_cert_initialized_ = false;
    printf("failed to set CA certificate\n");
}


void HttpClient::sini(struct altcp_pcb* pcb)
{
    //get mbdeTls context from PCB
    struct mbedtls_ssl_context *ssl = static_cast<struct mbedtls_ssl_context*>(altcp_tls_context(pcb));
    if (ssl) {
        // set hostname, sini
        int mbed_err = mbedtls_ssl_set_hostname(ssl, server_host_name_);

        if (mbed_err != 0) {
            printf("mbedtls_ssl_set_hostname failed: -0x%04X\n", -mbed_err);
        }
    }
}


void HttpClient::send_https_request(const char* method,const char* path,const char* body,const char* content_type,const HttpHeader* headers,size_t header_count)
{
    if (!ca_cert_initialized_){
        printf("No ce cerrt\n");
        return;
    }

    if (!get_connection_status()) {
        request_fail_ = true;
        ready_ = true;
        return; // Request fail!!
    }
    //copy path
    size_t len =0;
    if (path){
         len =  strlen(path);
        if (len >= PATH_MAX)
            len = PATH_MAX - 1;
        memcpy(request_path_, path, len);
    }
    request_path_[len] = '\0';

    //copy method
    len = 0;
    if (method){
        len = strlen(method) ;
        if (len>= MAX_REQUEST_METHOD_LEN)
            len = MAX_REQUEST_METHOD_LEN - 1;
        memcpy(request_method_, method, len);
    }
    request_method_[len] = '\0';

    //copy body
    len = 0;
    if (body){
        len = strlen(body);
        if (len >= MAX_REQUEST_BODY_LEN)
            len = MAX_REQUEST_BODY_LEN - 1;
        memcpy(request_body_,body,len);
    }
    request_body_[len] = '\0';

    //Copy content type
    len = 0;
    if (content_type){
        len = strlen(content_type);
        if (len >= MAX_CONTEN_TYPE_LEN)
            len = MAX_CONTEN_TYPE_LEN - 1;
        memcpy(content_type_,content_type,len);
    }
    content_type_[len] = '\0';


    // COPY HEADERS
    request_header_count_ = 0;

    if (headers && header_count!=0)
    {
        for (size_t i = 0; i < header_count && i < MAX_HEADERS; i++) {

            size_t klen = strlen(headers[i].key);
            if (klen >= MAX_HEADER_KEY_LEN)
                klen = MAX_HEADER_KEY_LEN - 1;
            memcpy(request_headers_[i].key, headers[i].key, klen);
            request_headers_[i].key[klen] = '\0';

            size_t vlen = strlen(headers[i].value);
            if (vlen >= MAX_HEADER_VALUE_LEN)
                vlen = MAX_HEADER_VALUE_LEN - 1;

            memcpy(request_headers_[i].value, headers[i].value, vlen);
            request_headers_[i].value[vlen] = '\0';

            request_header_count_++;
        }
    }

    ready_ = false;
    request_fail_ = false;
    buffer_index_ = 0;
    response_buffer_[0] = '\0';
    buffer_[0] = '\0';

    mbedtls_x509_crt cert;

    mbedtls_x509_crt_init(&cert);

    int ret = mbedtls_x509_crt_parse(
        &cert,
        (const unsigned char*)ca_cert_,
        cert_length_
    );

    printf("parse ret = %d\n", ret);


    // --- TLS PCB ---
    pcb_ = nullptr;
    // Create TLS configuration using CA certificate
    cyw43_arch_lwip_begin();

    tls_config_ = altcp_tls_create_config_client(ca_cert_,cert_length_);

    cyw43_arch_lwip_end();
    if (!tls_config_) {
        printf("tls_config_ creation failed\n");
        request_fail_ = true;
        ready_ = true;
        return;
    }

    // Allocate TLS PCB
    cyw43_arch_lwip_begin();
    pcb_ = altcp_tls_new(tls_config_, IPADDR_TYPE_V4);
    cyw43_arch_lwip_end();
    if (!pcb_) {
        altcp_tls_free_config(tls_config_);
        printf("tls PCB creation failed\n");
        request_fail_ = true;
        ready_ = true;
        return;
    }

    sini(pcb_);

    //set this object as callback argument
    cyw43_arch_lwip_begin();
    altcp_arg(pcb_, this);

    cyw43_arch_lwip_end();

    // Set callbacks
    cyw43_arch_lwip_begin();
    altcp_recv(pcb_, tls_recv_cb);

    altcp_err(pcb_, callback_altcp_err); // Optionally handle errors
    cyw43_arch_lwip_end();

    // Initiate TLS connection
    cyw43_arch_lwip_begin();
    err_t err = altcp_connect(pcb_, &server_ip_address, LWIP_IANA_PORT_HTTPS, callback_altcp_connect);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        altcp_tls_free_config(tls_config_);
        printf("altcp_connect failed with error code: %d\n", err);
        altcp_close(pcb_);
        request_fail_ = true;
        ready_ = true;
    }
}


err_t HttpClient::callback_altcp_connect(void* arg,struct altcp_pcb* pcb,err_t err)
{
    if (err != ERR_OK) {
        return err;
    }
    HttpClient* client = static_cast<HttpClient*>(arg);

    int offset = 0;
    char request[250];

    offset += snprintf(request + offset, sizeof(request) - offset,
        "%s %s HTTP/1.1\r\n"
        "Host: %s\r\n",
        client->request_method_,
        client->request_path_,
        client->get_server_host_name());

    if (client->content_type_[0] != '\0') {
        offset += snprintf(request + offset, sizeof(request) - offset,
            "Content-Type: %s\r\n",
            client->content_type_);
    }

    size_t body_len = strlen(client->request_body_);

    offset += snprintf(request + offset, sizeof(request) - offset,"Content-Length: %zu\r\n", body_len);

    // custom headers
    for (size_t i = 0; i < client->request_header_count_; i++) {
        offset += snprintf(request + offset, sizeof(request) - offset,
            "%s: %s\r\n",
            client->request_headers_[i].key,
            client->request_headers_[i].value);
    }

    offset += snprintf(request + offset, sizeof(request) - offset,
        "Connection: close\r\n"
        "\r\n");

    // body
    if (body_len > 0) {
        offset += snprintf(request + offset, sizeof(request) - offset,"%s",client->request_body_);
    }

    // Lähetä request PCB:lle
    err_t lwip_err = altcp_write(pcb, request, strlen(request), 0);
    if (lwip_err == ERR_OK) {
        altcp_output(pcb); //tyhjennä lähetyspuskuri
    } else {
        client->request_fail_ = true;
        client->ready_ = true;
    }
    return ERR_OK;
}


err_t HttpClient::callback_altcp_connect_get(void* arg,struct altcp_pcb* pcb,err_t err) //https
{
    if (err != ERR_OK) {
        return err;
    }

    HttpClient* client = static_cast<HttpClient*>(arg);

    char request[256];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             client->request_path_,
             client->get_server_host_name());

    // Lähetä request PCB:lle
    err_t lwip_err = altcp_write(pcb, request, strlen(request), 0);
    if (lwip_err == ERR_OK) {
        altcp_output(pcb); //tyhjennä lähetyspuskuri
    } else {
        client->request_fail_ = true;
        client->ready_ = true;
    }
    return ERR_OK;
}

void HttpClient::callback_altcp_err(void* arg, err_t err) //https
{
    HttpClient* client = static_cast<HttpClient*>(arg);
    client->request_fail_ = true;
    client->ready_ = true;
    printf("tls connection error: %d\n", err);
}

err_t HttpClient::tls_recv_cb(void* arg, struct altcp_pcb* apcb, struct pbuf* p, err_t err) { //https
    HttpClient* client = static_cast<HttpClient*>(arg);

    if (!p) {
        altcp_tls_free_config(client->tls_config_);

        altcp_close(apcb);
        client->handle_response();
        client->ready_ = true;
        return ERR_OK;
    }

    altcp_recved(apcb, p->len);

    if (p->len + client->buffer_index_ < RECV_BUF_SIZE) {
        memcpy(client->response_buffer_ + client->buffer_index_, p->payload, p->len);
        client->buffer_index_ += p->len;
        client->response_buffer_[client->buffer_index_] = '\0';
    } else {
        client->request_fail_ = true;
    }

    pbuf_free(p);
    return ERR_OK;
}


err_t HttpClient::tcp_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    HttpClient* client = static_cast<HttpClient*>(arg);

    if (err != ERR_OK) {
        return err;
    }

    char request[256];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             client->request_path_,
             client->get_server_host_name());

    tcp_write(tpcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    tcp_recv(tpcb, tcp_recv_cb);
    return ERR_OK;
}

void HttpClient::handle_response()
{
    char *body = strstr(response_buffer_, "\r\n\r\n");

    if (body) {
        body += 4;

        char *json_start = strchr(body, '{');
        char *json_end   = strrchr(body, '}');

        if (!json_start || !json_end || json_end <= json_start) { //can't find json, copy full message
            memcpy(buffer_,body,strlen(body));
            buffer_[strlen(body)] = '\0';
            return;
        }

        size_t json_len = json_end - json_start + 1;

        if (json_len >= RECV_BUF_SIZE) {
            request_fail_ = true;
            return;
        }

        memcpy(buffer_, json_start, json_len);
        buffer_[json_len] = '\0';
    }else{
        request_fail_ = true;
    }
}

err_t HttpClient::tcp_recv_cb(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err) {
    HttpClient* client = static_cast<HttpClient*>(arg);
     if (!p) {
        tcp_close(tpcb);
        client->handle_response();
        client->ready_ =true;
        return ERR_OK;
    }
    tcp_recved(tpcb, p->len);
    pbuf_free(p);

    //write payload to buffer
    if (p->len+client->buffer_index_ < RECV_BUF_SIZE) {
        memcpy(client->response_buffer_ + client->buffer_index_, p->payload, p->len);
        client->buffer_index_ += p->len;
        client->response_buffer_[client->buffer_index_] = '\0'; // Null-terminate the buffer
    } else {
        client->request_fail_ = true;
    }

    return ERR_OK;
}


void HttpClient::send_http_get_request(const char* path) {

    if (!get_connection_status()){
        request_fail_ = true; ready_ = true; return; //request fail!!
    }

    size_t len = strlen(path);
    if (len >= PATH_MAX)
        len = PATH_MAX - 1;

    memcpy(request_path_, path, len);
    request_path_[len] = '\0';

    ready_ = false; //init request status variables
    request_fail_ = false;
    buffer_index_ = 0;
    buffer_[0] = '\0';

    struct tcp_pcb* pcb = tcp_new();
    tcp_arg(pcb, this);

    tcp_connect(pcb, &server_ip_address, LWIP_IANA_PORT_HTTP, tcp_connected_cb);
}

void HttpClient::keepAlive()
{
    cyw43_arch_poll();
}

void HttpClient::abort_request()
{
    cyw43_arch_lwip_begin();

    if (pcb_) {
        altcp_abort(pcb_);
        pcb_ = nullptr;
    }

    cyw43_arch_lwip_end();

    request_fail_ = true;
    ready_ = true;
    printf("request_aborted\n");

}

