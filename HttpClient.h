//
// Created by ak on 12.2.2026.
//

#ifndef BLINK_HTTPCLIENT_H
#define BLINK_HTTPCLIENT_H

#include <cstddef>

#define RECV_BUF_SIZE 7048
#define MAX_CERTIFICATE_LEN 5024
#define MAX_REQUEST_METHOD_LEN 50
#define PATH_MAX 256
#define WIFI_TIMEOUT 5000

#define MAX_CONTEN_TYPE_LEN 50
#define  MAX_REQUEST_BODY_LEN 100
#define MAX_HEADERS 10

static const size_t MAX_HEADER_KEY_LEN = 64;
static const size_t MAX_HEADER_VALUE_LEN = 256;
#include "lwip/ip_addr.h"
#include "lwip/err.h"


bool connect_wifi_helper(const char* wifi_ssid, const char* wifi_password);
void disconnect_wifi_helper();
bool wifi_connected_helper();


struct HttpHeader {
     char key[50];
     char value[50];
};

class HttpClient
{
public:
    HttpClient(); //constructor

    void connect_to_server(const char* server_address);

    void send_https_request(const char* method,const char* path,const char* body = nullptr,const char* content_type = nullptr,const HttpHeader* headers = nullptr,size_t header_count = 0);
    void send_http_get_request(const char* path); //Todo update this

    void keepAlive(); //if you use freertos, don't use this
    void set_connection_status(bool status){server_connect_status = status;}
    void set_server_ip_address(ip_addr_t ip_address){(server_ip_address = ip_address);}
    ip_addr_t get_server_ip_address(){return server_ip_address;}
    const char* get_server_host_name(){return server_host_name_;}

    bool get_connection_status(){return server_connect_status;}
    void abort_request();

    const char* get_buffer(){return buffer_;}
    void clear_buffer(){buffer_[0] = '\0';}
    bool ready()const{return ready_;}
    bool request_fail()const{return request_fail_;}
    void set_ca_cert(const char* cert, size_t length);

private:
    char request_method_[MAX_REQUEST_METHOD_LEN];
    char request_path_[PATH_MAX];
    char content_type_[MAX_CONTEN_TYPE_LEN];

    HttpHeader request_headers_[MAX_HEADERS];
    size_t request_header_count_;

    bool wifi_initialized_;
    struct altcp_tls_config* tls_config_;
    err_t error_code;
    bool ready_;
    char request_body_[MAX_REQUEST_BODY_LEN];

    bool ca_cert_initialized_;
    u8_t ca_cert_[MAX_CERTIFICATE_LEN];
    size_t cert_length_;
    bool request_fail_;
    static void dns_cb(const char *name, const ip_addr_t *ipaddr, void *arg);

    static err_t tcp_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err);
    static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t tls_recv_cb(void* arg, struct altcp_pcb* apcb, struct pbuf* p, err_t err);
    static void callback_altcp_err(void* arg, err_t err);
    static err_t callback_altcp_connect_get(void* arg,struct altcp_pcb* pcb,err_t err);
    static err_t callback_altcp_connect(void* arg,struct altcp_pcb* pcb,err_t err);
    struct altcp_pcb* pcb_;

    void sini(struct altcp_pcb* pcb);

    char buffer_[RECV_BUF_SIZE];
    int buffer_index_;

    ip_addr_t server_ip_address;
    const char* server_host_name_;
    bool wifi_status;

    bool server_connect_status; //dns or any other step fail
};

#endif //BLINK_HTTPCLIENT_H