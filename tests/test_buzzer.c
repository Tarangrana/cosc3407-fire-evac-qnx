#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rpi_gpio.h>
#include <unistd.h>

#define BUZZER_PIN GPIO18

static bool init_buzzer(int gpio_pin)
{
    if (rpi_gpio_setup(gpio_pin, GPIO_OUT))
    {
        perror("rpi_gpio_setup");
        return false;
    }
    return true;
}

static bool buzzer_on(int gpio_pin)
{
    if (rpi_gpio_output(gpio_pin, GPIO_HIGH))
    {
        perror("rpi_gpio_output");
        return false;
    }
    return true;
}

static bool buzzer_off(int gpio_pin)
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
    if (!init_buzzer(BUZZER_PIN))
    {
        return EXIT_FAILURE;
    }

    printf("Buzzer test started...\n");

    while (1)
    {
        if (!buzzer_on(BUZZER_PIN))
        {
            return EXIT_FAILURE;
        }
        printf("Buzzer ON\n");
        usleep(500000);

        if (!buzzer_off(BUZZER_PIN))
        {
            return EXIT_FAILURE;
        }
        printf("Buzzer OFF\n");
        usleep(500000);
    }

    return EXIT_SUCCESS;
}