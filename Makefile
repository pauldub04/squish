ifeq ($(OS), Windows_NT)
	USER_OS := Windows
else
	USER_OS := $(shell uname -s)
endif

ifneq ($(USER_OS), Darwin)
	ASAN_OPTIONS := detect_leaks=1
endif

CC = cc
CFLAGS := -std=c99 -pedantic -g -fsanitize=address,undefined -Wall -Werror

TARGET := shell
SRC_DIR := src
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)


all: $(TARGET) test

test: $(TARGET)
	ASAN_OPTIONS=$(ASAN_OPTIONS) python3 test/test.py $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

-include $(DEPS)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean install
