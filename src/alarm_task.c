#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define BUZZER_PIN GPIO18

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

    int buzzer_active = 0;
    printf("[Alarm] Task started (HIGH priority)\n");

    while (1)
    {
        int current_fire;

        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        if (current_fire && !buzzer_active)
        {
            buzzer_on();
            buzzer_active = 1;
            printf("[Alarm] FIRE DETECTED! Buzzer ON\n");
        }
        else if (!current_fire && buzzer_active)
        {
            buzzer_off();
            buzzer_active = 0;
            printf("[Alarm] Fire cleared. Buzzer OFF\n");
        }

        sleep(1);
    }

    return NULL;
}#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define BUZZER_PIN GPIO18

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

    int buzzer_active = 0;
    printf("[Alarm] Task started (HIGH priority)\n");

    while (1)
    {
        int current_fire;

        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        if (current_fire && !buzzer_active)
        {
            buzzer_on();
            buzzer_active = 1;
            printf("[Alarm] FIRE DETECTED! Buzzer ON\n");
        }
        else if (!current_fire && buzzer_active)
        {
            buzzer_off();
            buzzer_active = 0;
            printf("[Alarm] Fire cleared. Buzzer OFF\n");
        }

        sleep(1);
    }

    return NULL;
}#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define BUZZER_PIN GPIO18

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

    int buzzer_active = 0;
    printf("[Alarm] Task started (HIGH priority)\n");

    while (1)
    {
        int current_fire;

        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        if (current_fire && !buzzer_active)
        {
            buzzer_on();
            buzzer_active = 1;
            printf("[Alarm] FIRE DETECTED! Buzzer ON\n");
        }
        else if (!current_fire && buzzer_active)
        {
            buzzer_off();
            buzzer_active = 0;
            printf("[Alarm] Fire cleared. Buzzer OFF\n");
        }

        sleep(1);
    }

    return NULL;
}