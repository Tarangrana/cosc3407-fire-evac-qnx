#include <stdio.h>
#include <unistd.h>
#include <rpi_gpio.h>

#define BUTTON_PIN GPIO5

int main()
{
    unsigned level;

    if (rpi_gpio_setup_pull(BUTTON_PIN, GPIO_IN, GPIO_PUD_UP))
    {
        printf("GPIO setup failed\n");
        return 1;
    }

    printf("Button test started...\n");

    while (1)
    {
        rpi_gpio_input(BUTTON_PIN, &level);

        if (level == 8)
            printf("Button PRESSED\n");
        else
            printf("Button RELEASED\n");

        usleep(200000);
    }

    return 0;
}