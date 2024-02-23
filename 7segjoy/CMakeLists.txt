include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_executable(7segjoy main.cpp)
target_include_directories(7segjoy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(7segjoy pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_enable_stdio_usb(7segjoy 1)
pico_enable_stdio_uart(7segjoy 0)
pico_add_extra_outputs(7segjoy)
