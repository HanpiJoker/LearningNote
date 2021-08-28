CROSS =
CC = $(CROSS)gcc
CXX = $(CROSS)g++
DEBUG =
CFLAGS = $(DEBUG) -Wall -Werror -W -c
LFLAGS = $(DEBUG) -Wall -Werror -W

ROOT = $(shell pwd)
INCLUDE_PATH = $(ROOT)/include
MODULES_PATH = $(ROOT)/modules
MODULES_DIR = $(shell find $(MODULES_PATH) -maxdepth 3 -type d)
MODULES = $(foreach dir, $(MODULES_DIR), $(wildcard $(dir)/*.c))

MODULE_OBJS = $(patsubst %.c, %.o, $(MODULES))
TARGET = main
TARGET_OBJS = $(TARGET).o

INCLUDE_FLAGS=-I$(INCLUDE_PATH)
################## Build ##################
all: $(TARGET)

$(TARGET): $(MODULE_OBJS) $(TARGET_OBJS)
	$(CC) $(LFLAGS) $^ -o $@ $(INCLUDE_FLAGS)

$(TARGET_OBJS): %.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_FLAGS)

$(MODULE_OBJS): %.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_FLAGS)

.PHONY: clean
clean:
	find . -name "*.o" -exec rm -f {} \;
	rm -f $(TARGET)
