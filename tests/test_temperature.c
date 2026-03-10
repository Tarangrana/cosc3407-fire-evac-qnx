#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <rpi_gpio.h>

#define DHT_PIN GPIO16

static void delay_us(long us)
{
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed =
            (now.tv_sec - start.tv_sec) * 1000000L +
            (now.tv_nsec - start.tv_nsec) / 1000L;
        if (elapsed >= us) {
            break;
        }
    } while (1);
}

static int read_level(unsigned *level)
{
    if (rpi_gpio_input(DHT_PIN, level)) {
        return -1;
    }
    return 0;
}

static int wait_for_state(unsigned target, int timeout_us)
{
    unsigned level = 0;

    while (timeout_us-- > 0) {
        if (read_level(&level) != 0) {
            return -1;
        }
        if (level == target) {
            return 0;
        }
        delay_us(1);
    }

    return -1;
}

static int measure_high_pulse_us(int max_us)
{
    unsigned level = 0;
    int count = 0;

    while (count < max_us) {
        if (read_level(&level) != 0) {
            return -1;
        }
        if (level == GPIO_LOW) {
            return count;
        }
        delay_us(1);
        count++;
    }

    return -2;
}

static int read_dht11(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};

    if (rpi_gpio_setup(DHT_PIN, GPIO_OUT)) {
        return -1;
    }

    if (rpi_gpio_output(DHT_PIN, GPIO_HIGH)) {
        return -1;
    }
    delay_us(50);

    if (rpi_gpio_output(DHT_PIN, GPIO_LOW)) {
        return -1;
    }
    delay_us(20000);

    if (rpi_gpio_output(DHT_PIN, GPIO_HIGH)) {
        return -1;
    }
    delay_us(40);

    if (rpi_gpio_setup_pull(DHT_PIN, GPIO_IN, GPIO_PUD_UP)) {
        return -1;
    }

    if (wait_for_state(GPIO_LOW, 120) != 0) return -1;
    if (wait_for_state(GPIO_HIGH, 120) != 0) return -1;
    if (wait_for_state(GPIO_LOW, 120) != 0) return -1;

    for (int i = 0; i < 40; i++) {
        if (wait_for_state(GPIO_HIGH, 120) != 0) {
            return -1;
        }

        int high_us = measure_high_pulse_us(120);

        if (high_us == -1) {
            return -1;
        }

        if (high_us == -2) {
            if (i == 39) {
                high_us = 8;
            } else {
                return -1;
            }
        }

        data[i / 8] <<= 1;
        if (high_us > 4) {
            data[i / 8] |= 1;
        }

        if (i != 39) {
            if (wait_for_state(GPIO_LOW, 120) != 0) {
                return -1;
            }
        }
    }

    uint8_t checksum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (checksum != data[4]) {
        return -1;
    }

    *humidity = (float)data[0];
    *temperature = (float)data[2];
    return 0;
}

static int get_stable_dht11(float *temp, float *hum)
{
    for (int attempt = 0; attempt < 5; attempt++) {
        if (read_dht11(temp, hum) == 0) {
            return 0;
        }
        usleep(500000);
    }
    return -1;
}

int main(void)
{
    printf("DHT11/GD test started on GPIO16 (D16)\n");
    printf("Press Ctrl+C to stop.\n");

    sleep(2);

    while (1) {
        float temp = 0.0f;
        float hum = 0.0f;

        if (get_stable_dht11(&temp, &hum) == 0) {
            printf("Temperature: %.0f C, Humidity: %.0f %%\n", temp, hum);
        } else {
            printf("Sensor read failed\n");
        }

        sleep(2);
    }

    return 0;
}