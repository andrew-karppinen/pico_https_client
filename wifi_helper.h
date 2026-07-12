

#ifndef BLINK_WIFI_HELPER_H
#define BLINK_WIFI_HELPER_H

bool wifi_connected_helper();
void disconnect_wifi_helper();
bool connect_wifi_helper(const char* wifi_ssid, const char* wifi_password,int wifi_timeout=5000);


#endif //BLINK_WIFI_HELPER_H
