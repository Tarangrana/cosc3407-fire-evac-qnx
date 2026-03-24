#include <stdio.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define RED_LED_PIN  GPIO22   // D22 = unsafe exit
#define BLUE_LED_PIN GPIO24   // D24 = safe exit

void* exit_task(void* arg)
{
    int local_exit_path;

    (void)arg;

    // Initialize red LED
    if (rpi_gpio_setup(RED_LED_PIN, GPIO_OUT) != 0)
    {
        perror("[Exit Task] Failed to initialize red LED pin");
        return NULL;
    }

    // Initialize blue LED
    if (rpi_gpio_setup(BLUE_LED_PIN, GPIO_OUT) != 0)
    {
        perror("[Exit Task] Failed to initialize blue LED pin");
        return NULL;
    }

    // Set both LEDs OFF initially
    if (rpi_gpio_output(RED_LED_PIN, GPIO_LOW) != 0)
    {
        perror("[Exit Task] Failed to set red LED initial state");
    }

    if (rpi_gpio_output(BLUE_LED_PIN, GPIO_LOW) != 0)
    {
        perror("[Exit Task] Failed to set blue LED initial state");
    }

    printf("[Exit Task] Red LED initialized on GPIO22.\n");
    printf("[Exit Task] Blue LED initialized on GPIO24.\n");

    while (1)
    {
        sem_wait(&fire_sem);
        local_exit_path = exit_path;
        sem_post(&fire_sem);

        if (local_exit_path == 1)
        {
            // Safe exit active
            if (rpi_gpio_output(BLUE_LED_PIN, GPIO_HIGH) != 0)
            {
                perror("[Exit Task] Failed to turn blue LED ON");
            }

            if (rpi_gpio_output(RED_LED_PIN, GPIO_LOW) != 0)
            {
                perror("[Exit Task] Failed to turn red LED OFF");
            }

            printf("[Exit Task] Safe exit route active -> Blue ON, Red OFF.\n");
        }
        else
        {
            // Unsafe / no emergency exit guidance
            if (rpi_gpio_output(BLUE_LED_PIN, GPIO_LOW) != 0)
            {
                perror("[Exit Task] Failed to turn blue LED OFF");
            }

            if (rpi_gpio_output(RED_LED_PIN, GPIO_HIGH) != 0)
            {
                perror("[Exit Task] Failed to turn red LED ON");
            }

            printf("[Exit Task] Unsafe exit state -> Red ON, Blue OFF.\n");
        }

        sleep(1);
    }

    return NULL;
}