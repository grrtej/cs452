#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"

void timer_task(void *parameters)
{
    while (true)
    {
        // Convert ticks to seconds: divide by configTICK_RATE_HZ
        // Convert seconds to ticks: multiply by configTICK_RATE_HZ
        printf(
            "Time since board powered on: %zu seconds\n",
            xTaskGetTickCount() / configTICK_RATE_HZ
        );
        vTaskDelay(1 * configTICK_RATE_HZ);
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(timer_task, "Timer Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {};
}
