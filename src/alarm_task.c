/*
 * alarm_task.c
 * Smart Fire Evacuation and Exit Guidance System
 * COSC3407 - Operating Systems I
 *
 * Purpose:
 * Activates the emergency buzzer when fire is detected.
 *
 * Source references:
 * - QNX Raspberry Pi GPIO sample library:
 *   https://gitlab.com/qnx/projects/hardware-component-samples
 *
 * Notes:
 * GPIO output control was adapted from QNX sample interfaces and
 * integrated with the project’s shared fire state so the alarm responds during emergencies.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define BUZZER_PIN GPIO18 // GPIO pin connected to the buzzer

static int buzzer_init(void)
{
    return rpi_gpio_setup(BUZZER_PIN, GPIO_OUT);
}

static void buzzer_on(void)
{
    rpi_gpio_output(BUZZER_PIN, GPIO_HIGH);
}

static void buzzer_off(void)
{
    rpi_gpio_output(BUZZER_PIN, GPIO_LOW);
}

void* alarm_task(void* arg)
{
    (void)arg;

    if (buzzer_init() != 0)
    {
        perror("[Alarm] buzzer init failed");
        return NULL;
    }

    int buzzer_active = 0; // keeps track of whether the buzzer is currently on or off
    printf("[Alarm] Task started (HIGH priority)\n");

    while (1)
    {
        int current_fire;

        // Using the semaphore to safely read the shared fire_status variable
        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        // if fire is detected and the buzzer is not already active, trigger the alarm
        if (current_fire && !buzzer_active)
        {
            buzzer_on();
            buzzer_active = 1;
            printf("[Alarm] FIRE DETECTED! Buzzer ON\n");
        }
        // if the fire condition clears and the buzzer was active, turn it off
        else if (!current_fire && buzzer_active)
        {
            buzzer_off();
            buzzer_active = 0;
            printf("[Alarm] Fire cleared. Buzzer OFF\n");
        }

         // a tiny pause so the loop does not consume CPU continuously
        sleep(1);
    }

    return NULL;
}