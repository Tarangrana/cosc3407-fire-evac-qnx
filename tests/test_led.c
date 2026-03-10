#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rpi_gpio.h>
#include <unistd.h>

#define GPIO_PIN GPIO5

static bool init_led(int gpio_pin)
{
    if (rpi_gpio_setup(gpio_pin, GPIO_OUT))
    {
        perror("rpi_gpio_setup");
        return false;
    }
    return true;
}

static bool led_on(int gpio_pin)
{
    if (rpi_gpio_output(gpio_pin, GPIO_HIGH))
    {
        perror("rpi_gpio_output");
        return false;
    }
    return true;
}

static bool led_off(int gpio_pin)
{
    if (rpi_gpio_output(gpio_pin, GPIO_LOW))
    {
        perror("rpi_gpio_output");
        return false;
    }
    return true;
}

int main(void)
{
    if (!init_led(GPIO_PIN))
    {
        return EXIT_FAILURE;
    }

    printf("LED test started...\n");

    while (1)
    {
        if (!led_off(GPIO_PIN))
        {
            return EXIT_FAILURE;
        }
        usleep(500000);

        if (!led_on(GPIO_PIN))
        {
            return EXIT_FAILURE;
        }
        usleep(500000);
    }

    return EXIT_SUCCESS;
}