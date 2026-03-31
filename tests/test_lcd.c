#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <devctl.h>
#include <hw/i2c.h>

#define LCD_ADDR 0x3E
#define I2C_DEV  "/dev/i2c1"   // if blank, change to /dev/i2c0

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
    int rc = i2c_send_bytes(fd, LCD_ADDR, msg, 2);
    if (rc != EOK) {
        printf("[LCD] cmd 0x%02X failed: %s\n", c, strerror(errno));
    }
}

static void lcd_data(int fd, uint8_t d) {
    uint8_t msg[2] = {0x40, d};
    int rc = i2c_send_bytes(fd, LCD_ADDR, msg, 2);
    if (rc != EOK) {
        printf("[LCD] data 0x%02X failed: %s\n", d, strerror(errno));
    }
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
    for (int i = 0; i < 16; i++) {
        lcd_data(fd, (uint8_t)line[i]);
    }
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

int main(void) {
    int fd = open(I2C_DEV, O_RDWR);
    if (fd < 0) {
        printf("Cannot open %s: %s\n", I2C_DEV, strerror(errno));
        return 1;
    }

    printf("LCD test on %s addr 0x%02X\n", I2C_DEV, LCD_ADDR);

    lcd_boot(fd);
    lcd_set_cursor(fd, 0, 0);
    lcd_print16(fd, "Vikas");
    lcd_set_cursor(fd, 1, 0);
    lcd_print16(fd, "Ki Mummy Cutie");

    sleep(5);

    close(fd);
    return 0;
}