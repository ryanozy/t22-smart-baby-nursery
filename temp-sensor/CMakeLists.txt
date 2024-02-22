cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

project(dht_example C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

pico_sdk_init()

add_subdirectory(dht)

# target_compile_definitions(dht_example PRIVATE
# WIFI_SSID=\"${WIFI_SSID}\"
# WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
# )

add_executable(dht_example dht_example.c)

pico_enable_stdio_uart(dht_example 1)
pico_enable_stdio_usb(dht_example 1)

pico_add_extra_outputs(dht_example)

target_compile_options(dht_example PRIVATE -Wall)

target_include_directories(dht_example PRIVATE
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
)

target_link_libraries(dht_example dht pico_stdlib pico_cyw43_arch_lwip_poll                 pico_lwip_mbedtls
pico_mbedtls
pico_lwip_mqtt)
#pico_lwip_iperf pico_cyw43_arch_lwip_poll 
