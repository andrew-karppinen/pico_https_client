

#ifndef BLINK_WIFI_HELPER_H
#define BLINK_WIFI_HELPER_H

bool is_wifi_connected_helper();
void disconnect_wifi_helper();
bool connect_wifi_helper(const char* wifi_ssid, const char* wifi_password,int wifi_timeout=5000);
//bool connect_wifi_freertos(const char* ssid,  const char* password);


#endif //BLINK_WIFI_HELPER_H
