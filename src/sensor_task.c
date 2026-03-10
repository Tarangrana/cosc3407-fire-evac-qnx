#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <devctl.h>
#include <hw/i2c.h>

#define LCD_ADDR 0x3E
#define I2C_DEV  "/dev/i2c1"   // if LCD stays blank, switch to "/dev/i2c0"

static int i2c_send_bytes(int fd, uint8_t addr7, const uint8_t *bytes, size_t n) {
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

static void lcd_cmd(int fd, uint8_t c) {
    uint8_t msg[2] = {0x80, c};
    i2c_send_bytes(fd, LCD_ADDR, msg, 2);
}

static void lcd_data(int fd, uint8_t d) {
    uint8_t msg[2] = {0x40, d};
    i2c_send_bytes(fd, LCD_ADDR, msg, 2);
}

static void lcd_clear(int fd) {
    lcd_cmd(fd, 0x01);
    usleep(2000);
}

static void lcd_set_cursor(int fd, int row, int col) {
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    lcd_cmd(fd, (uint8_t)(0x80 | (addr + (uint8_t)col)));
}

static void lcd_print16(int fd, const char *text) {
    char line[16];
    memset(line, ' ', sizeof(line));
    if (text) {
        size_t n = strlen(text);
        if (n > 16) n = 16;
        memcpy(line, text, n);
    }
    for (int i = 0; i < 16; i++) lcd_data(fd, (uint8_t)line[i]);
}

static void lcd_boot(int fd) {
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

static void lcd_show(int fd, const char *l1, const char *l2) {
    lcd_set_cursor(fd, 0, 0);
    lcd_print16(fd, l1);
    lcd_set_cursor(fd, 1, 0);
    lcd_print16(fd, l2);
}

static void msleep(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void* sensor_task(void* arg) {
    (void)arg;

    int fd = open(I2C_DEV, O_RDWR);
    if (fd < 0) {
        printf("[LCD] Cannot open %s: %s\n", I2C_DEV, strerror(errno));
        return NULL;
    }

    printf("[System] Starting controller on QNX...\n");
    printf("[LCD] Initializing Grove 16x2 LCD (addr 0x%02X on %s)\n", LCD_ADDR, I2C_DEV);
    lcd_boot(fd);

    lcd_show(fd, "SMART FIRE EVAC", "Status: BOOTING");
    printf("[System] Boot OK. Launching tasks...\n");
    sleep(2);

    int temp_c = 24;
    int fire = 0;

    // Demo script: normal -> warning -> fire -> evacuation
    for (int t = 0; t < 20; t++) {
        // “realistic” change
        if (t < 8) temp_c += 1;          // gentle rise
        else if (t < 12) temp_c += 3;    // faster rise
        else temp_c += 2;

        if (temp_c >= 55) fire = 1;

        if (!fire) {
            char l1[32], l2[32];
            snprintf(l1, sizeof(l1), "Temp:%2dC  SAFE", temp_c);
            snprintf(l2, sizeof(l2), "Alarm:OFF Exit:A");
            lcd_show(fd, l1, l2);

            printf("[Sensor] Temperature: %d C\n", temp_c);
            printf("[Alarm] Monitoring... (no fire)\n");
            printf("[Exit ] Safe route: Exit A\n\n");
        } else {
            char l1[32], l2[32];
            snprintf(l1, sizeof(l1), "Temp:%2dC FIRE!!", temp_c);
            snprintf(l2, sizeof(l2), "ALARM ON  Exit:B");
            lcd_show(fd, l1, l2);

            printf("[Sensor] Temperature: %d C\n", temp_c);
            printf("[Sensor] FIRE DETECTED! Threshold exceeded.\n");
            printf("[Alarm] Emergency alarm ACTIVATED!\n");
            printf("[Exit ] Directing people to Exit B\n\n");
        }

        sleep(1);
    }

    // Final “evacuate now” screen for video
    lcd_clear(fd);
    lcd_show(fd, "EVACUATE NOW!", "Proceed to B");
    printf("[System] Demo state reached: EVACUATE NOW.\n");
    msleep(2500);

    close(fd);
    return NULL;
}