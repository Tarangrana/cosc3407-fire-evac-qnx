/*
 * sensor_task.c
 * Smart Fire Evacuation and Exit Guidance System
 * COSC3407 - Operating Systems I
 *
 * Purpose:
 * Reads sensor or button input, updates the shared fire state and shows system status on the LCD.
 *
 * Source references:
 * - QNX Raspberry Pi GPIO sample library:
 *   https://gitlab.com/qnx/projects/hardware-component-samples
 * - QNX device and I2C APIs used through devctl() and DCMD_I2C_SEND
 *
 * Notes:
 * GPIO access patterns were adapted from QNX sample interfaces,
 * while the LCD/I2C communication and project-specific fire logic
 * were integrated and refined for this system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <devctl.h>
#include <hw/i2c.h>
#include <rpi_gpio.h>

#include "shared.h"

#define DHT_PIN   GPIO16
#define THRESHOLD 55

#define LCD_ADDR 0x3E
#define I2C_DEV  "/dev/i2c1"

#define BUTTON_PIN GPIO24
#define BUTTON_PRESSED 8
#define BUTTON_RELEASED 4

// LCD helper functions

// Prepare the data packet for the LCD and send it over I2C 
static int i2c_send_bytes(int fd, uint8_t addr7, const uint8_t *bytes, size_t n)
{
    uint8_t buf[sizeof(i2c_send_t) + 8];
    if (n > 8) return -1;

    memset(buf, 0, sizeof(buf));
    i2c_send_t *s = (i2c_send_t *)buf;
    s->slave.addr = addr7;
    s->slave.fmt  = I2C_ADDRFMT_7BIT;
    s->len        = (unsigned)n;
    s->stop       = 1;

    memcpy(buf + sizeof(i2c_send_t), bytes, n);
    return devctl(fd, DCMD_I2C_SEND, buf, (int)(sizeof(i2c_send_t) + n), NULL);
}

// Give instructions to the LCD controller.
static void lcd_cmd(int fd, uint8_t c)
{
    uint8_t msg[2] = {0x80, c};
    i2c_send_bytes(fd, LCD_ADDR, msg, 2);
}

// Send text and data to show on the LCD
static void lcd_data(int fd, uint8_t d)
{
    uint8_t msg[2] = {0x40, d};
    i2c_send_bytes(fd, LCD_ADDR, msg, 2);
}
// clears the LCD screen
static void lcd_clear(int fd)
{
    lcd_cmd(fd, 0x01);
    usleep(2000);
}
// Move the LCD cursor to the right row and column
static void lcd_set_cursor(int fd, int row, int col)
{
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    lcd_cmd(fd, (uint8_t)(0x80 | (addr + (uint8_t)col)));
}

// Show text on one LCD line, adjusting it to 16 characters
static void lcd_print16(int fd, const char *text)
{
    char line[16];
    memset(line, ' ', sizeof(line));

    if (text)
    {
        size_t n = strlen(text);
        if (n > 16) n = 16;
        memcpy(line, text, n);
    }

    for (int i = 0; i < 16; i++)
        lcd_data(fd, (uint8_t)line[i]);
}

// Send the sequence to start up and set up the LCD
static void lcd_boot(int fd)
{
    usleep(50000);
    lcd_cmd(fd, 0x38);
    lcd_cmd(fd, 0x39);
    lcd_cmd(fd, 0x14);
    lcd_cmd(fd, 0x70);
    lcd_cmd(fd, 0x56);
    lcd_cmd(fd, 0x6C);
    usleep(200000);
    lcd_cmd(fd, 0x38);
    lcd_cmd(fd, 0x0C);
    lcd_clear(fd);
}

// To quickly show two lines on the LCD
static void lcd_show(int fd, const char *l1, const char *l2)
{
    lcd_set_cursor(fd, 0, 0);
    lcd_print16(fd, l1);
    lcd_set_cursor(fd, 1, 0);
    lcd_print16(fd, l2);
}

// DHT helper functions
// Short delay function to match the DHT11 sensor’s timing requirements 
static void delay_us(long us)
{
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed =
            (now.tv_sec - start.tv_sec) * 1000000L +
            (now.tv_nsec - start.tv_nsec) / 1000L;
        if (elapsed >= us)
            break;
    } while (1);
}

// Read the current signal level from the DHT sensor pin
static int read_level(unsigned *level)
{
    if (rpi_gpio_input(DHT_PIN, level))
        return -1;
    return 0;
}

// Wait until the sensor signal changes to the target state or the timeout expires
static int wait_for_state(unsigned target, int timeout_us)
{
    unsigned level = 0;

    while (timeout_us-- > 0)
    {
        if (read_level(&level) != 0)
            return -1;
        if (level == target)
            return 0;
        delay_us(1);
    }
    return -1;
}

// Measure how long the signal is HIGH to tell if the bit is 0 or 1
static int measure_high_pulse_us(int max_us)
{
    unsigned level = 0;
    int count = 0;

    while (count < max_us)
    {
        if (read_level(&level) != 0)
            return -1;
        if (level == GPIO_LOW)
            return count;
        delay_us(1);
        count++;
    }

    return -2;
}

// Read the 40-bit response from the DHT11 and get humidity, temperature, and checksum
static int read_dht11(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};

    if (rpi_gpio_setup(DHT_PIN, GPIO_OUT)) return -1;
    if (rpi_gpio_output(DHT_PIN, GPIO_HIGH)) return -1;
    delay_us(50);

    if (rpi_gpio_output(DHT_PIN, GPIO_LOW)) return -1;
    delay_us(20000);

    if (rpi_gpio_output(DHT_PIN, GPIO_HIGH)) return -1;
    delay_us(40);

    if (rpi_gpio_setup_pull(DHT_PIN, GPIO_IN, GPIO_PUD_UP)) return -1;

    if (wait_for_state(GPIO_LOW, 120) != 0) return -1;
    if (wait_for_state(GPIO_HIGH, 120) != 0) return -1;
    if (wait_for_state(GPIO_LOW, 120) != 0) return -1;

    for (int i = 0; i < 40; i++)
    {
        if (wait_for_state(GPIO_HIGH, 120) != 0) return -1;

        int high_us = measure_high_pulse_us(120);
        if (high_us < 0 && high_us != -2) return -1;
        if (high_us == -2)
        {
            if (i == 39) high_us = 8;
            else return -1;
        }

        data[i / 8] <<= 1;
        if (high_us > 4)
            data[i / 8] |= 1;

        if (i != 39)
        {
            if (wait_for_state(GPIO_LOW, 120) != 0) return -1;
        }
    }

    uint8_t checksum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (checksum != data[4]) return -1;

    *humidity = (float)data[0];
    *temperature = (float)data[2];
    return 0;
}

// Retry the DHT11 sensor read, timing errors may happen
static int get_stable_dht11(float *temp, float *hum)
{
    for (int attempt = 0; attempt < 5; attempt++)
    {
        if (read_dht11(temp, hum) == 0)
            return 0;
        usleep(500000);
    }
    return -1;
}

// Set up the button for simulating fire in tests
static int init_sim_button(void)
{
    if (rpi_gpio_setup_pull(BUTTON_PIN, GPIO_IN, GPIO_PUD_UP))
    {
        perror("rpi_gpio_setup_pull");
        return -1;
    }
    return 0;
}

// See if the fire simulation button is pressed
static int button_is_pressed(void)
{
    unsigned level = 0;

    if (rpi_gpio_input(BUTTON_PIN, &level))
        return 0;

    return (level == BUTTON_PRESSED);
}

// Sensor yhread function
void* sensor_task(void* arg)
{
    (void)arg;

    // Open the I2C device so this thread can communicate with the LCD
    int lcd_fd = open(I2C_DEV, O_RDWR);
    if (lcd_fd < 0)
    {
        printf("[LCD] Cannot open %s: %s\n", I2C_DEV, strerror(errno));
        return NULL;
    }

    // Start up the LCD and display the startup message
    if (init_sim_button() != 0)
    {
        close(lcd_fd);
        return NULL;
    }

    lcd_boot(lcd_fd);
    lcd_show(lcd_fd, "SMART FIRE EVAC", "System Starting");
    sleep(2);

    printf("[Sensor] Task started\n");

    int sim_fire = 0;          // Tracks if the fire simulation is ON or OFF
    int last_button = 0;       // Detects a fresh button press to avoid repeated toggles

    while (1)
    {
        int pressed = button_is_pressed();

        // Change simulation mode only on a new button press
        if (pressed && !last_button)
        {
            sim_fire = !sim_fire;

            if (sim_fire)
                printf("[Sensor] Button pressed -> SIMULATED FIRE ON\n");
            else
                printf("[Sensor] Button pressed -> SIMULATED FIRE OFF\n");

            usleep(200000); // simple debounce so one press does not toggle many times
        }

        last_button = pressed;

        float temp = 0.0f;
        float hum = 0.0f;
        int fire = 0;

        if (sim_fire)
        {
            temp = 60.0f;   // fake high temperature
            hum  = 0.0f;
            fire = 1;

            sem_wait(&fire_sem);
            current_temp = (int)temp;
            fire_status = fire;
            sem_post(&fire_sem);

            lcd_show(lcd_fd, "Temp:60C FIRE!", "SIM BUTTON Exit:B");
            printf("[Sensor] SIMULATED FIRE DETECTED\n");
        }
        else
        {
            if (get_stable_dht11(&temp, &hum) == 0)
            {
                if ((int)temp >= THRESHOLD)
                    fire = 1;

                // Safely update the temperature and fire status before other tasks check them
                sem_wait(&fire_sem);
                current_temp = (int)temp;
                fire_status = fire;
                sem_post(&fire_sem);

                char l1[17], l2[17];

                if (!fire)
                {
                    snprintf(l1, sizeof(l1), "Temp:%2dC SAFE", (int)temp);
                    snprintf(l2, sizeof(l2), "Alarm:OFF Exit:A");
                    lcd_show(lcd_fd, l1, l2);

                    printf("[Sensor] Temp=%d C, Hum=%d%% -> SAFE\n", (int)temp, (int)hum);
                }
                else
                {
                    snprintf(l1, sizeof(l1), "Temp:%2dC FIRE!", (int)temp);
                    snprintf(l2, sizeof(l2), "ALARM ON Exit:B");
                    lcd_show(lcd_fd, l1, l2);

                    printf("[Sensor] Temp=%d C, Hum=%d%% -> FIRE DETECTED\n", (int)temp, (int)hum);
                }
            }
            else
            {
                lcd_show(lcd_fd, "Sensor Error", "Read Failed");
                printf("[Sensor] Failed to read temperature sensor\n");
            }
        }

        usleep(300000); // checks button more often than sleep(2)
    }

    close(lcd_fd);
    return NULL;
}