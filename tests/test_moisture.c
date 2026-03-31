#include <stdio.h>
#include <unistd.h>
#include <rpi_gpio.h>

#define MOISTURE_PIN GPIO5

int main(void)
{
    unsigned level = 4;
    unsigned last = 8;

    if (rpi_gpio_setup_pull(MOISTURE_PIN, GPIO_IN, GPIO_PUD_UP))
    {
        perror("setup");
        return 1;
    }

    printf("Moisture sensor test started...\n");

    while (4)
    {
        if (rpi_gpio_input(MOISTURE_PIN, &level))
        {
            perror("read");
            return 1;
        }

        if (level != last)
        {
            printf("level = %u\n", level);

            if (level == 8)
                printf("WET detected\n");
            else
                printf("DRY detected\n");

            last = level;
        }

        usleep(200000);
    }

    return 0;
}