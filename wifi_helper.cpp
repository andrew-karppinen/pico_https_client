
#include "pico/cyw43_arch.h"
#include "wifi_helper.h"


bool is_wifi_connected_helper()
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
    cyw43_arch_enable_sta_mode(); //enable wifi

    printf("Connecting to WiFi...\n");

    int status = cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_password,CYW43_AUTH_WPA2_AES_PSK, wifi_timeout);
    sleep_ms(1000);

    if (status)
    {
        printf("WiFi failed\n");
        return false;
    }else{
        printf("wifi ok");
        return  true;
    }
}

/*
 FreeRTOS example

#include "FreeRTOS.h"
#include "task.h"

bool connect_wifi_freertos(const char* ssid,  const char* password)
{

    //connect wifi with using cyw43_arch_wifi_connect_async function(FreeRTOS compatibility)
    //waiting max 20sec



    cyw43_arch_enable_sta_mode(); //enable wifi

    printf("Connecting to WiFi...\n");

    cyw43_arch_lwip_begin();
    int err = cyw43_arch_wifi_connect_async(ssid, password, CYW43_AUTH_WPA2_AES_PSK); //async,
    cyw43_arch_lwip_end();

    if (err != 0) {
        return false;
    }

    int status = CYW43_LINK_DOWN;
    for (int i = 0; i < 100; i++) {
        // 20 s max
        status = cyw43_tcpip_link_status(
            &cyw43_state,
            CYW43_ITF_STA
        );


        if (status == CYW43_LINK_UP) {
            return true;
        }

        //stop if link status is fail, nonet or badauth
        if (status == CYW43_LINK_FAIL ||
            status == CYW43_LINK_NONET ||
            status == CYW43_LINK_BADAUTH)
        {
            return false;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
    return false;

}
*/