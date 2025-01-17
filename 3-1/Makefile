CC=mpicc
CFLAGS=-Wall -O3
INCLUDES=-I include
BUILD_DIR=build
BIN_DIR=bin
TARGET=$(BIN_DIR)/game_of_life

SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEPS=$(wildcard include/*.h)

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

$(BUILD_DIR)/%.o: src/%.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET) $(BIN_DIR)/*

clean-test:
	rm -f $ test/fig/* test/results/*

.PHONY: all clean dirs
