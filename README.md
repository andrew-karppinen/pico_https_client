# c++ HTTP Library for Raspberry Pi Pico W / Pico 2 W

Simple c++ HTTP/HTTPS client library for Raspberry Pi Pico W / Pico 2 W using Pico SDK + lwIP (+ mbedTLS for HTTPS).

## Features

- Custom request headers
- HTTP and HTTPS support
- Lightweight API for embedded projects

---

## Requirements

- Raspberry Pi Pico SDK
- Pico W / Pico 2 W board support
- lwIP
- mbedTLS (for HTTPS)

---

## Project structure

- `http_library/HttpClient.h` / `http_library/HttpClient.cpp`  
  Main client implementation
- `http_library/wifi_helper.h` / `http_library/wifi_helper.cpp`  
  Wi-Fi helper functions
- `lwipopts.h` (project root)
- `mbedtls_config.h` (project root)

---

## Top-level CMake example

Your main project should include Pico SDK, add this library as subdirectory, and define mbedTLS config:

```cmake
cmake_minimum_required(VERSION 3.12)
set(PICO_BOARD pico2_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(my_app C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Make lwipopts.h visible for lwIP build
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

pico_sdk_init()

add_subdirectory(http_library)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    MBEDTLS_CONFIG_FILE="mbedtls_config.h"
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_mbedtls
    pico_lwip_mbedtls
    http_library
)

pico_add_extra_outputs(${PROJECT_NAME})
```

---

## `http_library/CMakeLists.txt` (library)

The library links Pico/lwIP/mbedTLS dependencies, including CYW43 arch (poll variant in this example).



---

## Quick start example

```cpp
#include "HttpClient.h"
#include "wifi_helper.h"



int main() {
    stdio_init_all();

    if (!connect_wifi_helper("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD", 15000)) {
        printf("Wi-Fi connect failed\n");
        return 1;
    }

    HttpClient client = HttpClient(); //create httpclient object
    client.set_ca_cert(PICOHTTPS_CA_GTS_ROOT4,sizeof(PICOHTTPS_CA_GTS_ROOT4)); //set cert
    const char* path = "/";
    
    client.connect_to_server("server_ip_or_domain");
    while (!client.is_ready()) {
        client.keep_alive(); //needed for lwIP poll architecture
    }
    
    if (!client.get_connection_status()){ //check connection status
        return 1;
    }
    
    client.send_http_request("GET", path, nullptr, nullptr,nullptr,0);


    while (!client.is_ready()) {
        client.keep_alive(); 
    }
    
    //Check whether the request failed:
    if (client.request_fail()) {
      return 1;
    }

    
    //get response:
    printf("%s", client.get_buffer());
    

    return 0;
}
```
## send_http_request() and send_https_request() share the same function prototype:
> void send_https_request(const char* method,const char* path,const char* body = nullptr,const char* content_type = nullptr,const HttpHeader* headers = nullptr,size_t header_count = 0);


---

## Custom request headers
```cpp
HttpHeader rheaders[1] = {0};
strcpy(rheaders[0].key, "Header_key");
strcpy(rheaders[0].value, "value");

client.send_http_request("POST", path, nullptr, nullptr,rheaders,1);

```

--- 

### Callbacks

By default, received data is stored in an internal buffer and parsed automatically.

You can override this by registering callbacks. This is useful for streaming large
responses (for example firmware updates) without buffering the entire response.

`set_data_callback()` is called for each received data chunk:

```cpp
client.set_data_callback([](const uint8_t* data, size_t len, void* arg) {
    static_cast<FirmwareUpdater*>(arg)->feed(data, len);
}, &updater);
```

`set_done_callback()` is called once when the response is complete:

```cpp
client.set_done_callback([](void* arg) {
    static_cast<FirmwareUpdater*>(arg)->finalize();
});
```

> **Note:** If callbacks are registered, `get_buffer()` will be empty.
> If no callbacks are registered, the response is available via `get_buffer()`
> after `is_ready()` returns `true`.

---

### Example CA certificate

```
//google gts root 4
#define PICOHTTPS_CA_GTS_ROOT4 "-----BEGIN CERTIFICATE-----\n\
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD\n\
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG\n\
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw\n\
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz\n\
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi\n\
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi\n\
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR\n\
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW\n\
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D\n\
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8\n\
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD\n\
-----END CERTIFICATE-----\n"
```
---

## Notes

- `lwipopts.h` must be available in include path (current setup expects it at project root).
- `mbedtls_config.h` is selected via:
    - `target_compile_definitions(... MBEDTLS_CONFIG_FILE="mbedtls_config.h")`

- If you are not using the polling architecture, edit the library's `CMakeLists.txt` and replace:
    - `pico_cyw43_arch_lwip_poll`
      with
    - `pico_cyw43_arch_lwip_sys_freertos`
    
---

## License

This project is licensed under the MIT License. See the LICENSE file for details.

This project depends on third-party libraries (such as Pico SDK, lwIP, and mbedTLS), which are distributed under their own licenses.