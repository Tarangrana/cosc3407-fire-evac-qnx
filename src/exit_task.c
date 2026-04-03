/*
 * exit_task.c
 * Smart Fire Evacuation and Exit Guidance System
 * COSC3407 - Operating Systems I
 *
 * Purpose:
 * Controls LED indicators that guide occupants toward the safest exit.
 *
 * Source references:
 * - QNX Raspberry Pi GPIO sample library
 *
 * Notes:
 * LED control logic was adapted from QNX GPIO examples and customized
 * for evacuation routing in this project.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpi_gpio.h>
#include "shared.h"

#define EXIT_A_LED GPIO22   // LED for Exit A
#define EXIT_B_LED GPIO5    // LED for Exit B

// Start both LED pins as GPIO outputs 
static int leds_init(void)
{
    if (rpi_gpio_setup(EXIT_A_LED, GPIO_OUT) != 0)
        return -1;
    if (rpi_gpio_setup(EXIT_B_LED, GPIO_OUT) != 0)
        return -1;
    return 0;
}

// To Turn on Exit A LED and turn off Exit B LED 
static void set_exit_a(void)
{
    rpi_gpio_output(EXIT_A_LED, GPIO_HIGH);
    rpi_gpio_output(EXIT_B_LED, GPIO_LOW);
}

// To Turn on Exit B LED and turn off Exit A LED
static void set_exit_b(void)
{
    rpi_gpio_output(EXIT_A_LED, GPIO_LOW);
    rpi_gpio_output(EXIT_B_LED, GPIO_HIGH);
}

void* exit_task(void* arg) 
{
    (void)arg;   

     // Set up the LED before starting the loop to ensure they are ready to indicate the exit status
    if (leds_init() != 0)
    {
        perror("[Exit] LED init failed");
        return NULL;
    }

    printf("[Exit] Task started (LOW priority)\n");

    while (1)
    {
         // Use the semaphore to safely check the shared fire state 
        int current_fire;

        sem_wait(&fire_sem);
        current_fire = fire_status;
        sem_post(&fire_sem);

        // Update the exits based on the fire state
        sem_wait(&fire_sem);
        if (current_fire)
        {
            // When a fire is detected, send people to Exit B
            exit_path = 1;
            set_exit_b();
            printf("[Exit] EMERGENCY: Directing to Exit B\n");
        }
        else
        {
            // In normal conditions, use Exit A
            exit_path = 0;
            set_exit_a();
            printf("[Exit] Normal: Exit A available\n");
        }
        sem_post(&fire_sem);

        // a short delay so the LEDs don’t update continuously
        sleep(2);
    }

    return NULL;
}