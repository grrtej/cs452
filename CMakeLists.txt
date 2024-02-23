cmake_minimum_required(VERSION 3.25)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
set(PICO_BOARD adafruit_feather_rp2040)

project(cs452 C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(blink)
add_subdirectory(hellopico)
add_subdirectory(7segjoy)
