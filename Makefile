CC = gcc
CFLAGS = -Wall -g -O2 
LDFLAGS = -lraylib -lm -pthread -ldl -lrt -lGL -lX11

ifndef DEMO
$(error Uso: make run DEMO=billar)
endif

PROJECT_DIR = $(DEMO)
SRC_DIR = $(PROJECT_DIR)/src
INCLUDE_DIR = $(PROJECT_DIR)/include
BUILD_DIR = $(PROJECT_DIR)/build

CFLAGS += -I$(INCLUDE_DIR)

SRCS := $(shell find $(SRC_DIR) -name '*.c')

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGET = $(BUILD_DIR)/$(DEMO)

.PHONY: all clean run dir_check

all: dir_check $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@./$(TARGET)

dir_check:
	@if [ ! -d "$(PROJECT_DIR)" ]; then \
		echo "Error: La carpeta '$(PROJECT_DIR)' no existe."; \
		exit 1; \
	fi

clean:
	rm -rf $(BUILD_DIR)