SRC = src
BUILD = .build
BIN = $(BUILD)/bin
OBJ = $(BUILD)/obj
INCLUDE = include

CC = gcc
CC_FLAGS = -std=gnu99 -Wall -Wextra

$(SRC):
	mkdir -p $(SRC) 
$(BUILD):
	mkdir -p $(BUILD)
$(BIN): $(BUILD)
	mkdir -p $(BIN)
$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

MAIN_TARGET = $(SRC)/main.c
MAIN_TARGET_BIN = $(BIN)/tester
MAIN_TARGET_OBJ = $(OBJ)/tester.o

HELPERS_TARGET = $(SRC)/helpers.c
HELPERS_TARGET_OBJ = $(OBJ)/helpers.o

FUNCS = \
	$(SRC)/__region_log_error.c \
	$(SRC)/__region_alloc.c \
	$(SRC)/__region_alloc_item.c \

REGION_OBJ = ../.build/obj/region.o

$(REGION_OBJ):
	make -C .. build 

$(HELPERS_TARGET_OBJ): $(OBJ) $(HELPERS_TARGET) $(REGION_OBJ)
	$(CC) $(CC_FLAGS) -c $(HELPERS_TARGET) -o $(HELPERS_TARGET_OBJ)

$(MAIN_TARGET): $(HELPERS_TARGET_OBJ) $(BIN) $(REGION_OBJ)
	$(CC) $(CC_FLAGS) $(MAIN_TARGET) $(HELPERS_TARGET_OBJ) $(FUNCS) $(REGION_OBJ) -o $(MAIN_TARGET_BIN)

build: $(BUILD) $(OBJ) $(BIN) $(MAIN_TARGET) $(REGION_OBJ)

run: $(MAIN_TARGET)
	./.build/bin/tester

clean:
	rm -rf $(BUILD) 
