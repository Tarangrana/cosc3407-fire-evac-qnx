#include <stdio.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define BUZZER_PIN GPIO18

void* alarm_task(void* arg)
{
    int local_fire_status;

    (void)arg;

    if (rpi_gpio_setup(BUZZER_PIN, GPIO_OUT) != 0)
    {
        perror("[Alarm Task] Failed to configure buzzer output");
        return NULL;
    }

    if (rpi_gpio_output(BUZZER_PIN, GPIO_LOW) != 0)
    {
        perror("[Alarm Task] Failed to initialize buzzer OFF");
    }

    printf("[Alarm Task] Buzzer initialized on GPIO18.\n");

    while (1)
    {
        sem_wait(&fire_sem);
        local_fire_status = fire_status;
        sem_post(&fire_sem);

        if (local_fire_status == 1)
        {
            if (rpi_gpio_output(BUZZER_PIN, GPIO_HIGH) != 0)
            {
                perror("[Alarm Task] Failed to turn buzzer ON");
            }
            printf("[Alarm Task] FIRE ALERT! Buzzer ON.\n");
        }
        else
        {
            if (rpi_gpio_output(BUZZER_PIN, GPIO_LOW) != 0)
            {
                perror("[Alarm Task] Failed to turn buzzer OFF");
            }
            printf("[Alarm Task] System normal. Alarm OFF.\n");
        }

        sleep(1);
    }

    return NULL;
}