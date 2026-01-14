CC = gcc
ifdef OS
   RM = cmd /c del /Q
   EXT = .exe
   
   RAYLIB_PATH = C:/Users/LENOVO/raylib
   
   CFLAGS = -Wall -g -O2 -I$(RAYLIB_PATH)/include
   LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm
else
   RM = rm -f
   EXT = 
   CFLAGS = -Wall -g -O2 
   LDFLAGS = -lraylib -lm -pthread -ldl -lrt -lGL -lX11
endif

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

TARGET = $(BUILD_DIR)/$(DEMO)$(EXT)

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
	$(RM) $(OBJS) $(TARGET)