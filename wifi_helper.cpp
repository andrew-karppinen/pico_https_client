
#include "pico/cyw43_arch.h"
#include "wifi_helper.h"


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

bool connect_wifi_helper(const char* wifi_ssid, const char* wifi_password,int wifi_timeout)
{
    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return false;
    }
    cyw43_arch_enable_sta_mode(); //enable wifi

    printf("Connecting to WiFi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_password,CYW43_AUTH_WPA2_AES_PSK, wifi_timeout))
    {
        printf("WiFi failed\n");
        return false;
    }else{
        return  true;
    }
}



