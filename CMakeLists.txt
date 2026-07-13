# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall
        -Wno-format
        -Wno-unused-function
        -Wno-maybe-uninitialized
)

# Build as a static library, not an executable
add_library(http_library STATIC
        HttpClient.cpp
        HttpClient.h
        wifi_helper.cpp
        wifi_helper.h

)

target_include_directories(http_library PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/
)

target_link_libraries(http_library
        pico_stdlib
        pico_util
        pico_lwip_mbedtls
        pico_mbedtls

        #SELECT ONE CYW43 architecture:
        pico_cyw43_arch_lwip_poll
        #pico_cyw43_arch_lwip_sys_freertos
        #(other CYW43 architectures may also work)
)