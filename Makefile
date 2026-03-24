VARIANT := gcc_ntoaarch64le
QNX_IP := 192.168.10.2
QNX_USER := root
QNX_DIR := /root

CC := qcc
TARGET := fire_evac
SRCS := src/main.c src/sensor_task.c src/alarm_task.c src/exit_task.c

# GPIO
GPIO_INC := -ICommon/system/gpio -ICommon/rpi_gpio/public
GPIO_CFLAGS := -V$(VARIANT) -Wall -Wextra -O2 -g $(GPIO_INC)

GPIO_PLATFORM := aarch64le
GPIO_PROFILE := debug
GPIO_BUILD_DIR := Common/rpi_gpio/build/$(GPIO_PLATFORM)-$(GPIO_PROFILE)
GPIO_LDFLAGS := -L$(GPIO_BUILD_DIR)
GPIO_LDLIBS := -lrpi_gpio -lm

# I2C
I2C_INC := -Ihardware-component-samples/Common/rpi_i2c/public
I2C_CFLAGS := -V$(VARIANT) -Wall -Wextra -O2 -g $(I2C_INC)

I2C_PLATFORM := aarch64le
I2C_PROFILE := debug
I2C_BUILD_DIR := hardware-component-samples/Common/rpi_i2c/build/$(I2C_PLATFORM)-$(I2C_PROFILE)
I2C_LDFLAGS := -L$(I2C_BUILD_DIR)
I2C_LDLIBS := -lrpi_i2c -lm

# Final application flags
APP_INC := -Iinclude $(GPIO_INC)
APP_CFLAGS := -V$(VARIANT) -Wall -Wextra -O2 -g $(APP_INC)
APP_LDFLAGS := $(GPIO_LDFLAGS)
APP_LDLIBS := $(GPIO_LDLIBS)

.PHONY: all gpio i2c clean ssh \
        deploy_led deploy_button deploy_buzzer deploy_temperature deploy_lcd \
        deploy_app run_app

all: gpio i2c test_led test_button test_buzzer test_temperature $(TARGET)

gpio:
	$(MAKE) -C hardware-component-samples/common/rpi_gpio PLATFORM=$(GPIO_PLATFORM) BUILD_PROFILE=$(GPIO_PROFILE)

i2c:
	$(MAKE) -C hardware-component-samples/common/rpi_i2c PLATFORM=$(I2C_PLATFORM) BUILD_PROFILE=$(I2C_PROFILE)

$(TARGET): $(SRCS)
	$(CC) $(APP_CFLAGS) -o $@ $(SRCS) $(APP_LDFLAGS) $(APP_LDLIBS)

test_led: tests/test_led.c
	$(CC) $(GPIO_CFLAGS) -o $@ $< $(GPIO_LDFLAGS) $(GPIO_LDLIBS)

test_button: tests/test_button.c
	$(CC) $(GPIO_CFLAGS) -o $@ $< $(GPIO_LDFLAGS) $(GPIO_LDLIBS)

test_buzzer: tests/test_buzzer.c
	$(CC) $(GPIO_CFLAGS) -o $@ $< $(GPIO_LDFLAGS) $(GPIO_LDLIBS)

test_temperature: tests/test_temperature.c
	$(CC) $(GPIO_CFLAGS) -o $@ $< $(GPIO_LDFLAGS) $(GPIO_LDLIBS)

test_lcd: tests/test_lcd.c
	$(CC) -V$(VARIANT) -Wall -Wextra -O2 -g -o $@ $<

deploy_lcd: test_lcd
	tar -cf - test_lcd | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x test_lcd"

deploy_led: test_led
	tar -cf - test_led | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x test_led"

deploy_button: test_button
	tar -cf - test_button | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x test_button"

deploy_buzzer: test_buzzer
	tar -cf - test_buzzer | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x test_buzzer"

deploy_temperature: test_temperature
	tar -cf - test_temperature | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x test_temperature"

deploy_app: $(TARGET)
	tar -cf - $(TARGET) | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x $(TARGET)"

run_app: deploy_app
	ssh -tt $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && ./$(TARGET)"

ssh:
	ssh $(QNX_USER)@$(QNX_IP)

clean:
	-del /q test_led test_button test_buzzer test_temperature test_lcd $(TARGET) 2>NUL || exit 0