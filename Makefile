CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc -Ithird_party
LDFLAGS = -lm -lpthread -ldl

SRC_DIR   = src
BUILD_DIR = build
BIN       = $(BUILD_DIR)/nvidia_rma

APP_SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/database.c \
	$(SRC_DIR)/utils.c \
	$(SRC_DIR)/cliente.c \
	$(SRC_DIR)/produto.c \
	$(SRC_DIR)/estoque.c \
	$(SRC_DIR)/rma.c \
	$(SRC_DIR)/relatorios.c

APP_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_SRCS))
SQLITE_OBJ = $(BUILD_DIR)/sqlite3.o

.PHONY: all build run clean

all: build

build: $(BIN)

$(BUILD_DIR)/.stamp:
	mkdir -p $(BUILD_DIR) db
	touch $(BUILD_DIR)/.stamp

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)/.stamp
	$(CC) $(CFLAGS) -c $< -o $@

$(SQLITE_OBJ): third_party/sqlite3.c | $(BUILD_DIR)/.stamp
	$(CC) -std=c11 -O2 -Ithird_party -DSQLITE_THREADSAFE=1 \
		-DSQLITE_OMIT_LOAD_EXTENSION -c $< -o $@

$(BIN): $(APP_OBJS) $(SQLITE_OBJ)
	$(CC) $(CFLAGS) $(APP_OBJS) $(SQLITE_OBJ) -o $(BIN) $(LDFLAGS)

run: build
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)
