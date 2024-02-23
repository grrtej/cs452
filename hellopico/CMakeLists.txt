include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_executable(hellopico main.cpp)
target_include_directories(hellopico PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hellopico pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_enable_stdio_usb(hellopico 1)
pico_enable_stdio_uart(hellopico 0)
pico_add_extra_outputs(hellopico)
