set(PICO_SDK_FREERTOS_SOURCE $ENV{PICO_SDK_PATH}/../FreeRTOS-Kernel) # works on my machine

add_library(freertos
    ${PICO_SDK_FREERTOS_SOURCE}/event_groups.c
    ${PICO_SDK_FREERTOS_SOURCE}/list.c
    ${PICO_SDK_FREERTOS_SOURCE}/queue.c
    ${PICO_SDK_FREERTOS_SOURCE}/stream_buffer.c
    ${PICO_SDK_FREERTOS_SOURCE}/tasks.c
    ${PICO_SDK_FREERTOS_SOURCE}/timers.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/MemMang/heap_3.c
    ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(freertos PUBLIC
    .
    ${PICO_SDK_FREERTOS_SOURCE}/include
    ${PICO_SDK_FREERTOS_SOURCE}/portable/GCC/ARM_CM0
)

add_executable(hellopico main.cpp)
target_link_libraries(hellopico pico_stdlib freertos)
pico_enable_stdio_usb(hellopico 1)
pico_enable_stdio_uart(hellopico 0)
pico_add_extra_outputs(hellopico)
