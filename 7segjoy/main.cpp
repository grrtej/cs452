#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void counter_task(void *param);
void led_task(void *param);

int main()
{
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    xTaskCreate(counter_task, "COUNTER_TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) { };
}

// Oscillate between 42 and -42. Step to the next number every 0.5 second.
// Blink the D13 LED on each step by sending an event to the LED task.
void counter_task(void *param)
{
    // Create the LED task and pass the queue to it.
    bool led_queue_msg;
    auto led_queue = xQueueCreate(1, sizeof led_queue_msg);
    xTaskCreate(led_task, "LED_TASK", configMINIMAL_STACK_SIZE, led_queue, 1, NULL);

    int x = 42;
    int dx = 1;

    auto last_wake_time = xTaskGetTickCount();
    while (true)
    {
        // LED ON
        led_queue_msg = true;
        xQueueSend(led_queue, &led_queue_msg, 0);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(250));

        printf("COUNTER_TASK: x = %d\n", x);

        if (x == 42 || x == -42)
        {
            dx = -dx; // flip
        }
        x += dx; // step

        // LED OFF
        led_queue_msg = false;
        xQueueSend(led_queue, &led_queue_msg, 0);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(250));
    }
}

// Receive an event from the counter task to blink the D13 LED.
// NOTE: Cannot use delay.
void led_task(void *param)
{
    bool led_queue_msg;
    auto led_queue = static_cast<QueueHandle_t>(param);

    while (true)
    {
        if (xQueueReceive(led_queue, &led_queue_msg, 0))
        {
            gpio_put(PICO_DEFAULT_LED_PIN, led_queue_msg);
        }
    }
}
