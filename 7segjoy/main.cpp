#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define SS_A 26
#define SS_B 27
#define SS_C 29
#define SS_D 18
#define SS_E 25
#define SS_F 7
#define SS_G 28
#define SS_DP 24
#define SS_CC1 11
#define SS_CC2 10
#define LED PICO_DEFAULT_LED_PIN

void init();
void counter_task(void *param);
void led_task(void *param);
void misc_task(void *param);

int main()
{
    init();

    // xTaskCreate(counter_task, "COUNTER_TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(misc_task, "MISC_TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) { };
}

// Oscillate between 42 and -42. Step to the next number every 0.5 second.
// Blink the D13 LED on each step by sending an event to the LED task.
void counter_task(void *param)
{
    // Create a queue and pass it to the LED task.
    bool led_queue_msg;
    auto led_queue = xQueueCreate(1, sizeof led_queue_msg);
    xTaskCreate(led_task, "LED_TASK", configMINIMAL_STACK_SIZE, led_queue, 1, NULL);

    int x = 42;
    int dx = 1;

    auto last_wake_time = xTaskGetTickCount();
    while (true)
    {
        printf("COUNTER_TASK: x = %d\n", x);
        if (x == 42 || x == -42)
        {
            dx = -dx; // flip
        }
        x += dx; // step

        // LED On
        led_queue_msg = true;
        xQueueSend(led_queue, &led_queue_msg, 0);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(250));

        // LED Off
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
            gpio_put(LED, led_queue_msg);
        }
    }
}

// Test the 7-segment display
void misc_task(void *param)
{
    gpio_put(SS_CC1, false);
    gpio_put(SS_CC2, false);

    auto last_wake_time = xTaskGetTickCount();
    while (true)
    {
        gpio_put(SS_A, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_A, false);

        gpio_put(SS_B, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_B, false);

        gpio_put(SS_C, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_C, false);

        gpio_put(SS_D, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_D, false);

        gpio_put(SS_E, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_E, false);

        gpio_put(SS_F, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_F, false);

        gpio_put(SS_G, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_G, false);

        gpio_put(SS_DP, true);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
        gpio_put(SS_DP, false);
    }
}

void init()
{
    stdio_init_all();

    gpio_init(SS_A);
    gpio_init(SS_B);
    gpio_init(SS_C);
    gpio_init(SS_D);
    gpio_init(SS_E);
    gpio_init(SS_F);
    gpio_init(SS_G);
    gpio_init(SS_DP);
    gpio_init(SS_CC1);
    gpio_init(SS_CC2);
    gpio_init(LED);

    gpio_set_dir(SS_A, GPIO_OUT);
    gpio_set_dir(SS_B, GPIO_OUT);
    gpio_set_dir(SS_C, GPIO_OUT);
    gpio_set_dir(SS_D, GPIO_OUT);
    gpio_set_dir(SS_E, GPIO_OUT);
    gpio_set_dir(SS_F, GPIO_OUT);
    gpio_set_dir(SS_G, GPIO_OUT);
    gpio_set_dir(SS_DP, GPIO_OUT);
    gpio_set_dir(SS_CC1, GPIO_OUT);
    gpio_set_dir(SS_CC2, GPIO_OUT);
    gpio_set_dir(LED, GPIO_OUT);
}
