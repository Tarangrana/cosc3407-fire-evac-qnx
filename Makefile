VARIANT := gcc_ntoaarch64le
QNX_IP := 192.168.10.2
QNX_USER := root
QNX_DIR := /root

CC := qcc
TARGET := fire_evac

GPIO_INC := -Ihardware-component-samples/common/rpi_gpio/public
I2C_INC  := -Ihardware-component-samples/common/rpi_i2c/public
SYS_GPIO_INC := -Ihardware-component-samples/common/system/gpio
INC := -Iinclude $(GPIO_INC) $(I2C_INC) $(SYS_GPIO_INC)

CFLAGS := -V$(VARIANT) -Wall -Wextra -O2 -g $(INC)

GPIO_PLATFORM := aarch64le
GPIO_PROFILE := debug
GPIO_BUILD_DIR := hardware-component-samples/common/rpi_gpio/build/$(GPIO_PLATFORM)-$(GPIO_PROFILE)

I2C_PLATFORM := aarch64le
I2C_PROFILE := debug
I2C_BUILD_DIR := hardware-component-samples/common/rpi_i2c/build/$(I2C_PLATFORM)-$(I2C_PROFILE)

LDFLAGS := -L$(GPIO_BUILD_DIR) -L$(I2C_BUILD_DIR)
LDLIBS := -lrpi_gpio -lrpi_i2c -lm 

SRCS := src/main.c src/sensor_task.c src/alarm_task.c src/exit_task.c

.PHONY: all gpio i2c deploy run ssh clean

all: gpio i2c $(TARGET)

gpio:
	$(MAKE) -C hardware-component-samples/common/rpi_gpio PLATFORM=$(GPIO_PLATFORM) BUILD_PROFILE=$(GPIO_PROFILE)

i2c:
	$(MAKE) -C hardware-component-samples/common/rpi_i2c PLATFORM=$(I2C_PLATFORM) BUILD_PROFILE=$(I2C_PROFILE)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS) $(LDLIBS)

deploy: $(TARGET)
	tar -cf - $(TARGET) | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x $(TARGET)"

run: deploy
	ssh -tt $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && ./$(TARGET)"

ssh:
	ssh $(QNX_USER)@$(QNX_IP)

clean:
	-del /q $(TARGET) 2>NUL || exit 0