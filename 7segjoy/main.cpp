#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();
    while (true) {
        printf("hi from pico\n");
        sleep_ms(1000);
    }
}
