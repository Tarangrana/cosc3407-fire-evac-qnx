VARIANT := gcc_ntoaarch64le
QNX_IP := 192.168.10.2
QNX_USER := root
QNX_DIR := /root

TARGET := fire_evac
SRCS := src/main.c src/sensor_task.c

# IMPORTANT: your folder is "Common" (capital C)
INC := -ICommon/system/gpio -ICommon/rpi_gpio/public

CFLAGS := -V$(VARIANT) -Wall -Wextra -O2 -g $(INC)

# rpi_gpio builds a static library into its build folder
GPIO_PLATFORM := aarch64le
GPIO_PROFILE  := debug
GPIO_BUILD_DIR := Common/rpi_gpio/build/$(GPIO_PLATFORM)-$(GPIO_PROFILE)

LDFLAGS := -L$(GPIO_BUILD_DIR)
LDLIBS  := -lrpi_gpio -lm

.PHONY: all gpio deploy run ssh clean

all: gpio $(TARGET)

# Build the rpi_gpio helper library first
gpio:
	$(MAKE) -C Common/rpi_gpio PLATFORM=$(GPIO_PLATFORM) BUILD_PROFILE=$(GPIO_PROFILE)

$(TARGET): $(SRCS)
	qcc $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS) $(LDLIBS)

deploy: $(TARGET)
	tar -cf - $(TARGET) | ssh $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && tar -xf - && /proc/boot/chmod +x $(TARGET)"

run: deploy
	ssh -tt $(QNX_USER)@$(QNX_IP) "cd $(QNX_DIR) && ./$(TARGET)"

ssh:
	ssh $(QNX_USER)@$(QNX_IP)

clean:
	-del /q $(TARGET) 2>NUL || exit 0