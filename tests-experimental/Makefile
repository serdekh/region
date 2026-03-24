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

build: $(OBJ) $(REGION_SOBJ)
	$(CC) -shared -o $(OBJ)/common.so -fPIC $(FUNCS)/common.c
	$(CC) -shared -o $(OBJ)/__region_alloc.so -fPIC $(FUNCS)/__region_alloc.c $(OBJ)/common.so

run: build $(BIN)
	$(CC) $(FRAMEWORK_C) $(SRC)/main.c -o $(FRAMEWORK_EXEC) -ldl
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
