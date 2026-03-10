#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>

#define BUTTON_PIN GPIO5

int main(void)
{
    if (rpi_gpio_setup_pull(BUTTON_PIN, GPIO_IN, GPIO_PUD_DOWN))
    {
        perror("rpi_gpio_setup_pull");
        return EXIT_FAILURE;
    }

    printf("Button test on GPIO5 (D5)\n");
    printf("Press and release the button.\n");

    while (1)
    {
        unsigned level = 0;

        if (rpi_gpio_input(BUTTON_PIN, &level))
        {
            perror("rpi_gpio_input");
            return EXIT_FAILURE;
        }

        if (level == GPIO_HIGH)
            printf("PRESSED\n");
        else if (level == GPIO_LOW)
            printf("RELEASED\n");
        else
            printf("RAW LEVEL = %u\n", level);

        usleep(300000);
    }

    return EXIT_SUCCESS;
}