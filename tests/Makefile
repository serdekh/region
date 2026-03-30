CC = gcc

SRC = src
FUNCS = $(SRC)/funcs
INCLUDE = $(SRC)/include
TESTER = $(SRC)/tester

BUILD = .build
OBJ = $(BUILD)/obj
BIN = $(BUILD)/bin

FRAMEWORK_C = $(TESTER)/tester.c
FRAMEWORK_H = $(TESTER)/tester.c
FRAMEWORK_EXEC = $(BIN)/tester

REGION_SOBJ = ../.build/obj/region.so

$(BUILD):
	mkdir -p .build

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

$(REGION_SOBJ):
	make -C .. build-shared

FUNCS_SRCS := $(shell find $(FUNCS) -name '*.c')
FUNCS_SRCS := $(filter-out $(FUNCS)/common/common.c,$(FUNCS_SRCS))

FUNCS_SOS := $(patsubst $(FUNCS)/%.c,$(OBJ)/%.so,$(FUNCS_SRCS))

build: $(OBJ) $(OBJ)/common/common.so $(FUNCS_SOS) $(REGION_SOBJ)

$(OBJ)/common:
	mkdir -p $(OBJ)/common

$(OBJ)/common/common.so: $(FUNCS)/common/common.c $(OBJ)/common
	$(CC) -shared -fPIC -o $@ $<

$(OBJ)/%.so: $(FUNCS)/%.c $(OBJ)/common/common.so
	$(CC) -shared -fPIC -o $@ $< $(OBJ)/common/common.so

run: build $(BIN)
	$(CC) $(FRAMEWORK_C) $(SRC)/main.c -o $(FRAMEWORK_EXEC) -ldl
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
