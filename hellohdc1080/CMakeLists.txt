include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_executable(hellohdc1080 main.cpp)
target_include_directories(hellohdc1080 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hellohdc1080 pico_stdlib hardware_i2c FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_enable_stdio_usb(hellohdc1080 1)
pico_enable_stdio_uart(hellohdc1080 0)
pico_add_extra_outputs(hellohdc1080)
