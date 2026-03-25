#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define EXIT_A_LED GPIO5
#define EXIT_B_LED GPIO27

static int leds_init(void)
{
    if (rpi_gpio_setup(EXIT_A_LED, GPIO_OUT) != 0)
        return -1;
    if (rpi_gpio_setup(EXIT_B_LED, GPIO_OUT) != 0)
        return -1;
    return 0;
}

static void set_exit_a(void)
{
    rpi_gpio_output(EXIT_A_LED, GPIO_HIGH);
    rpi_gpio_output(EXIT_B_LED, GPIO_LOW);
}

static void set_exit_b(void)
{
    rpi_gpio_output(EXIT_A_LED, GPIO_LOW);
    rpi_gpio_output(EXIT_B_LED, GPIO_HIGH);
}

void* exit_task(void* arg)
{
    (void)arg;

    if (leds_init() != 0)
    {
        perror("[Exit] LED init failed");
        return NULL;
    }

    printf("[Exit] Task started (LOW priority)\n");

    while (1)
    {
        int current_fire;

        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        sem_wait(&fire_sem);
        if (current_fire)
        {
            exit_path = 1;
            set_exit_b();
            printf("[Exit] EMERGENCY: Directing to Exit B\n");
        }
        else
        {
            exit_path = 0;
            set_exit_a();
            printf("[Exit] Normal: Exit A available\n");
        }
        sem_post(&fire_sem);

        sleep(2);
    }

    return NULL;
}#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define EXIT_A_LED GPIO5
#define EXIT_B_LED GPIO27

static int leds_init(void)
{
    if (rpi_gpio_setup(EXIT_A_LED, GPIO_OUT) != 0)
        return -1;
    if (rpi_gpio_setup(EXIT_B_LED, GPIO_OUT) != 0)
        return -1;
    return 0;
}

static void set_exit_a(void)
{
    rpi_gpio_output(EXIT_A_LED, GPIO_HIGH);
    rpi_gpio_output(EXIT_B_LED, GPIO_LOW);
}

static void set_exit_b(void)
{
    rpi_gpio_output(EXIT_A_LED, GPIO_LOW);
    rpi_gpio_output(EXIT_B_LED, GPIO_HIGH);
}

void* exit_task(void* arg)
{
    (void)arg;

    if (leds_init() != 0)
    {
        perror("[Exit] LED init failed");
        return NULL;
    }

    printf("[Exit] Task started (LOW priority)\n");

    while (1)
    {
        int current_fire;

        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        sem_wait(&fire_sem);
        if (current_fire)
        {
            exit_path = 1;
            set_exit_b();
            printf("[Exit] EMERGENCY: Directing to Exit B\n");
        }
        else
        {
            exit_path = 0;
            set_exit_a();
            printf("[Exit] Normal: Exit A available\n");
        }
        sem_post(&fire_sem);

        sleep(2);
    }

    return NULL;
}