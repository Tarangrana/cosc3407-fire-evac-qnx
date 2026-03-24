#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <rpi_gpio.h>

#define BUTTON_PIN GPIO5

static bool init_button(int pin)
{
    if (rpi_gpio_setup(pin, GPIO_IN))
    {
        perror("rpi_gpio_setup");
        return false;
    }

    return true;
}

int main(void)
{
    if (!init_button(BUTTON_PIN))
        return EXIT_FAILURE;

    printf("Button test started...\n");

    unsigned level = 0;
    unsigned last_level = 2; // impossible value to detect first change

    while (1)
    {
        if (rpi_gpio_input(BUTTON_PIN, &level))
        {
            perror("rpi_gpio_input");
            return EXIT_FAILURE;
        }

        if (level != last_level)
        {
            if (level == 0)
                printf("Button PRESSED\n");
            else
                printf("Button RELEASED\n");

            last_level = level;
        }

        usleep(100000);
    }

    return EXIT_SUCCESS;
}