#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

void timer_task(void *parameters)
{
    while (true)
    {
        printf("Time since board powered on: %u seconds\n",
                pdTICKS_TO_MS(xTaskGetTickCount()) / 1000);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(timer_task, "TIMER_TASK", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {};
}
