

#ifndef BLINK_HTTPCLIENT_H
#define BLINK_HTTPCLIENT_H

#include <cstddef>
#include "lwip/prot/iana.h"         // HTTPS port number


#define RECV_BUF_SIZE 7048
#define MAX_CERTIFICATE_LEN 5024
#define MAX_REQUEST_METHOD_LEN 50
#define PATH_MAX 256

#define MAX_REQUEST_LEN 300
#define MAX_CONTEN_TYPE_LEN 50
#define MAX_REQUEST_BODY_LEN 100
#define MAX_HEADERS 10

static const size_t MAX_HEADER_KEY_LEN = 64;
static const size_t MAX_HEADER_VALUE_LEN = 256;
#include "lwip/ip_addr.h"
#include "lwip/err.h"


typedef void (*data_callback_t)(const uint8_t* data, size_t len, void* arg);
typedef void (*done_callback_t)(void* arg);


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
    void send_http_request(const char* method,const char* path,const char* body = nullptr,const char* content_type = nullptr,const HttpHeader* headers = nullptr,size_t header_count = 0);

    void keep_alive(); //Only needed in poll mode (pico_cyw43_arch_lwip_poll)
    void set_connection_status(bool status){server_connect_status = status;}
    void set_server_ip_address(ip_addr_t ip_address){(server_ip_address = ip_address);}
    ip_addr_t get_server_ip_address(){return server_ip_address;}
    const char* get_server_host_name(){return server_host_name_;}

    bool is_connected() const { return server_connect_status; }
    bool request_succeeded() const { return !request_fail_; }
    void abort_request();

    const char* get_buffer(){return response_buffer_;}
    void clear_buffer(){response_buffer_[0] = '\0';}
    bool ready()const{return ready_;}
    void set_ca_cert(const char* cert, size_t length);

    //set calbacks
    void set_data_callback(data_callback_t cb, void* arg);
    void set_done_callback(done_callback_t cb);
    void clear_callbacks();

    void set_https_port(uint16_t port) { https_port_ = port; }
    void set_http_port(uint16_t port) { http_port_ = port; }
private:
    char request_method_[MAX_REQUEST_METHOD_LEN];
    char request_path_[PATH_MAX];
    char content_type_[MAX_CONTEN_TYPE_LEN];
    uint16_t https_port_ = LWIP_IANA_PORT_HTTPS; //default 443
    uint16_t http_port_ = LWIP_IANA_PORT_HTTP;   //default 80

    HttpHeader request_headers_[MAX_HEADERS];
    size_t request_header_count_;

    bool wifi_initialized_;
    struct altcp_tls_config* tls_config_;
    bool ready_;
    char request_body_[MAX_REQUEST_BODY_LEN];

    char response_buffer_[RECV_BUF_SIZE];
    void handle_response();

    bool ca_cert_initialized_;
    u8_t ca_cert_[MAX_CERTIFICATE_LEN];
    size_t cert_length_;
    bool request_fail_;
    static void dns_cb(const char *name, const ip_addr_t *ipaddr, void *arg);

    //HTTP
    static err_t handle_tcp_connected_cb(void *arg, struct tcp_pcb *tpcb, err_t err);
    static err_t handle_tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static void  handle_tcp_err_cb(void* arg, err_t err);

    //HTTPS
    static err_t tls_recv_cb(void* arg, struct altcp_pcb* apcb, struct pbuf* p, err_t err);
    static void handle_altcp_err(void* arg, err_t err);
    static err_t handle_altcp_connect(void* arg,struct altcp_pcb* pcb,err_t err);
    struct altcp_pcb* pcb_;

    void sini(struct altcp_pcb* pcb);

    int buffer_index_;

    data_callback_t callback_data_cb_ = nullptr;
    done_callback_t callback_done_cb_ = nullptr;
    void* cb_arg_ = nullptr;

    ip_addr_t server_ip_address;
    const char* server_host_name_;
    bool wifi_status;

    bool server_connect_status; //false if dns or any other step fail
};

#endif //BLINK_HTTPCLIENT_H