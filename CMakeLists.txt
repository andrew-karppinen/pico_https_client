# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Set board type because we are building for PicoW
set(PICO_BOARD pico2_w)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(blink C CXX ASM)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Pakotetaan koko projekti (Myös Pico SDK) käyttämään sinun konffiasi
add_compile_definitions(MBEDTLS_CONFIG_FILE="mbedtls_config.h")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
        HttpClient.cpp
        HttpClient.h
        mbedtls_config.h
        main.cpp

)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/
)
# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})



# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        pico_aon_timer
        pico_util
        pico_cyw43_arch_lwip_poll
        pico_stdlib
        hardware_pwm
        hardware_gpio
        pico_lwip_mbedtls
        pico_mbedtls
)

# Disable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
