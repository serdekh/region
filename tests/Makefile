CC = gcc

SRC = src
FUNCS = $(SRC)/funcs
INCLUDE = $(SRC)/include
TESTER = $(SRC)/tester

BUILD = .build
OBJ = $(BUILD)/obj
BIN = $(BUILD)/bin

FRAMEWORK = tester
FRAMEWORK_C = $(TESTER)/$(FRAMEWORK).c
FRAMEWORK_H = $(TESTER)/$(FRAMEWORK).c
FRAMEWORK_EXEC = $(BIN)/$(FRAMEWORK)

REGION_SOBJ = ../.build/sobj/region-test.so

$(BUILD):
	mkdir -p .build

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

$(REGION_SOBJ):
	make -C .. build-shared-test

FUNCS_SRCS := $(shell find $(FUNCS) -name '*.c')

$(OBJ)/%.so: $(FUNCS)/%.c $(OBJ)
	$(CC) -shared -fPIC -o $@ $<

FUNCS_SOS := $(patsubst $(FUNCS)/%.c,$(OBJ)/%.so,$(FUNCS_SRCS))

$(FRAMEWORK_EXEC): $(BUILD) $(OBJ) $(BIN) $(FUNCS_SOS)
	$(CC) $(FRAMEWORK_C) $(SRC)/main.c -o $(FRAMEWORK_EXEC) -ldl

build: $(REGION_SOBJ) $(OBJ) $(BIN) $(FUNCS_SOS) $(FRAMEWORK_EXEC)

run: $(FRAMEWORK_EXEC) $(REGION_SOBJ)
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
