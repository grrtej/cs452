// Include FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

// Include Pico C SDK
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

// Include Standard Library
#include <cstdio>    // for printf
#include <vector>    // for vector
#include <cmath>     // for abs
#include <algorithm> // for clamp

// tasks
void hdc1080_task(void *param);
void print_task(void *param);
void ss_task(void *param);

// helper functions
void hw_init();
void hdc1080_set_config(uint16_t config);
uint16_t hdc1080_get_value(uint8_t src);
void ss_set_digit(uint number, char side);

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
static uint16_t config;
static uint16_t manufacturer;
static uint16_t serial_first;
static uint16_t serial_mid;
static uint16_t serial_last;
static float celsius = 0;
static float fahrenheit = 0;
static float humidity = 0;

int main()
{
    // setup hdc1080 and the seven segment display
    hw_init();

    // create rtos resources
    xTaskCreate(hdc1080_task, "hdc1080_task", configMINIMAL_STACK_SIZE, nullptr, 2, nullptr);
    xTaskCreate(print_task, "print_task", configMINIMAL_STACK_SIZE, nullptr, 2, nullptr);
    xTaskCreate(ss_task, "ss_task", configMINIMAL_STACK_SIZE, nullptr, 1, nullptr);

    // let's go
    vTaskStartScheduler();
    while (true) { };
}

void hdc1080_task(void *param)
{
    uint16_t c;
    uint16_t h;

    auto last_wake_time = xTaskGetTickCount();
    while (true)
    {
        config = hdc1080_get_value(0x02);
        manufacturer = hdc1080_get_value(0xFE);
        serial_first = hdc1080_get_value(0xFB);
        serial_mid = hdc1080_get_value(0xFC);
        serial_last = hdc1080_get_value(0xFD);

        // get temperature and humidity sequentially
        std::vector<uint8_t> read(4);
        std::vector<uint8_t> write{0x00};
        i2c_write_blocking(i2c_default, 0x40, write.data(), write.size(), false);
        vTaskDelay(pdMS_TO_TICKS(20));
        i2c_read_blocking(i2c_default, 0x40, read.data(), read.size(), false);
        c = read[0] << 8 | read[1];
        h = read[2] << 8 | read[3];
        celsius = (static_cast<float>(c) / 65536) * 165.0 - 40.0;
        fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
        humidity = (static_cast<float>(h) / 65536) * 100.0;

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
    }
}

void print_task(void *param)
{
    auto last_wake_time = xTaskGetTickCount();
    while (true)
    {
        printf("%16s -> %#x\n", "Configuration", config);
        printf("%16s -> %#x\n", "Manufacturer ID", manufacturer);
        printf("%16s -> %x-%x-%x\n", "Serial Number", serial_first, serial_mid, serial_last);
        printf("               ──────────               \n");
        printf("\033[0;33m%16s\033[0m -> %.2f °C, %.2f °F\n", "Temperature", celsius, fahrenheit);
        printf("\033[0;34m%16s\033[0m -> %.2f%%\n", "Humidity", humidity);
        printf("────────────────────────────────────────\n");
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(10000));
    }
}

void hdc1080_set_config(uint16_t config)
{
    std::vector<uint8_t> write;
    write.push_back(0x02);
    write.push_back(config >> 8);
    write.push_back(config & 0xff);
    i2c_write_blocking(i2c_default, 0x40, write.data(), write.size(), false);
}

uint16_t hdc1080_get_value(uint8_t src)
{
    std::vector<uint8_t> read(2);
    std::vector<uint8_t> write{src};
    i2c_write_blocking(i2c_default, 0x40, write.data(), write.size(), false);
    i2c_read_blocking(i2c_default, 0x40, read.data(), read.size(), false);
    return read[0] << 8 | read[1];
}

void hw_init()
{
    stdio_init_all();

    // init seven-segment display
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

    // for picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    // init hdc1080
    i2c_init(i2c_default, 100 * 1000); // 100 kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // configure hdc1080
    hdc1080_set_config(0x9000); // soft reset, read temp and rh together

    // wait for hdc1080 to start
    // this is before the rtos kicks in, so can't use vTaskDelay() here
    sleep_ms(15);
}

// display x on the seven segment display
// target: 50 Hz (1000/50 = 20 ms)
// here, 6 * 3 = 18 ms (close to 20 ms)
void ss_task(void *param)
{
    int n;
    while (true)
    {
        n = std::clamp(static_cast<int>(humidity), -99, 99);

        ss_set_digit(abs(n) / 10, 'l');
        vTaskDelay(pdMS_TO_TICKS(6));
        ss_set_digit(abs(n) % 10, 'r');
        vTaskDelay(pdMS_TO_TICKS(6));
        // turn right decimal point on if negative
        gpio_put(SS_DP, n < 0);
        vTaskDelay(pdMS_TO_TICKS(6));
    }
}

void ss_set_digit(uint number, char side)
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
