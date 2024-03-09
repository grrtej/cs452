// Include FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Include Pico C SDK
#include "pico/stdlib.h"

// Include Standard Library
#include <cstdio>
#include <vector>
#include <cmath>

// tasks
void counter_task(void *param);
void led_task(void *param);
void ss_task(void *param);

// helper functions
void ss_set(uint number, char side);
void hw_init();

// constants
const uint SS_A = 26;
const uint SS_B = 27;
const uint SS_C = 29;
const uint SS_D = 18;
const uint SS_E = 25;
const uint SS_F = 7;
const uint SS_G = 28;
const uint SS_DP = 24;
const uint SS_CC1 = 11;
const uint SS_CC2 = 10;
const uint LED = PICO_DEFAULT_LED_PIN;
const std::vector<std::vector<uint>> SS_FONT{
    {SS_A, SS_B, SS_C, SS_D, SS_E, SS_F},       // 0
    {SS_B, SS_C},                               // 1
    {SS_A, SS_B, SS_G, SS_E, SS_D},             // 2
    {SS_A, SS_B, SS_G, SS_C, SS_D},             // 3
    {SS_F, SS_G, SS_B, SS_C},                   // 4
    {SS_A, SS_F, SS_G, SS_C, SS_D},             // 5
    {SS_A, SS_F, SS_E, SS_D, SS_C, SS_G},       // 6
    {SS_A, SS_B, SS_C},                         // 7
    {SS_A, SS_B, SS_C, SS_D, SS_E, SS_F, SS_G}, // 8
    {SS_A, SS_B, SS_C, SS_D, SS_F, SS_G}        // 9
};

// globals
static int x = 42;
static SemaphoreHandle_t semaphore;

int main()
{
    // setup seven segment display and d13 led
    hw_init();

    // create rtos resources
    xTaskCreate(counter_task, "counter_task", configMINIMAL_STACK_SIZE, nullptr, 1, nullptr);
    xTaskCreate(led_task, "led_task", configMINIMAL_STACK_SIZE, nullptr, 1, nullptr);
    xTaskCreate(ss_task, "ss_task", configMINIMAL_STACK_SIZE, nullptr, 1, nullptr);
    semaphore = xSemaphoreCreateBinary();

    // let's go
    vTaskStartScheduler();
    while (true) { };
}

// increases or decreases x every 500 ms
void counter_task(void *param)
{
    int dx = 1;
    auto last_wake_time = xTaskGetTickCount();
    while (true)
    {
        printf("Counter Task: x = %d\n", x);
        if (x == 42 || x == -42)
        {
            dx = -dx; // flip
        }
        x += dx; // step

        xSemaphoreTake(semaphore, 0);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
    }
}

// led blinking synchronized with counting
void led_task(void *param)
{
    bool state = true;
    while (true)
    {
        if (xSemaphoreGive(semaphore))
        {
            gpio_put(LED, state);
            state = !state;
        }
    }
}

// display x on the seven segment display
// target: 50 Hz (1000/50 = 20 ms)
// here, 6 * 3 = 18 ms (close to 20 ms)
void ss_task(void *param)
{
    while (true)
    {
        ss_set(abs(x) / 10, 'l');
        vTaskDelay(pdMS_TO_TICKS(6));
        ss_set(abs(x) % 10, 'r');
        vTaskDelay(pdMS_TO_TICKS(6));
        // turn right decimal point on if negative
        gpio_put(SS_DP, x < 0);
        vTaskDelay(pdMS_TO_TICKS(6));
    }
}

void ss_set(uint number, char side)
{
    // clear display
    for (auto segment : SS_FONT[8])
    {
        gpio_put(segment, false);
    }
    gpio_put(SS_DP, false);

    // turn correct segments for number on
    for (auto segment : SS_FONT[number])
    {
        gpio_put(segment, true);
    }

    // choose left or right display
    switch (side)
    {
    case 'l':
        gpio_put(SS_CC1, false);
        gpio_put(SS_CC2, true);
        break;
    case 'r':
        gpio_put(SS_CC1, true);
        gpio_put(SS_CC2, false);
        break;
    default: // turn off
        gpio_put(SS_CC1, true);
        gpio_put(SS_CC2, true);
        break;
    }
}

void hw_init()
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
