#include <stdio.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define FIRE_SENSOR_PIN GPIO16

void* sensor_task(void* arg)
{
    (void)arg;

    if (rpi_gpio_setup_pull(FIRE_SENSOR_PIN, GPIO_IN, GPIO_PUD_DOWN) != 0)
    {
        perror("[Sensor Task] Failed to configure fire sensor pin");
        return NULL;
    }

    printf("[Sensor Task] Fire sensor initialized on GPIO16.\n");

    while (1)
    {
        unsigned level = 0;

        if (rpi_gpio_input(FIRE_SENSOR_PIN, &level) != 0)
        {
            perror("[Sensor Task] Failed to read fire sensor input");
            usleep(500000);
            continue;
        }

        sem_wait(&fire_sem);

        if (level == GPIO_LOW)
        {
            fire_status = 1;
            exit_path = 1;
        }
        else
        {
            fire_status = 0;
            exit_path = 0;
        }

        sem_post(&fire_sem);

        if (level == GPIO_LOW)
        {
            printf("[Sensor Task] Fire detected -> fire_status=1, exit_path=1\n");
        }
        else
        {
            printf("[Sensor Task] No fire -> fire_status=0, exit_path=0\n");
        }

        sleep(1);
    }

    return NULL;
}