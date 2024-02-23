#include "pico/stdlib.h"

void blink(int ms);

int main()
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true) {
        // ...---...
        blink(250);
        blink(250);
        blink(250);
        blink(500);
        blink(500);
        blink(500);
        blink(250);
        blink(250);
        blink(250);

        // pause for a bit
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(1000);
    }
}

void blink(int ms)
{
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(ms);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    sleep_ms(ms);
}
