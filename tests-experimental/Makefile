CC = gcc

SRC = src
FUNCS = $(SRC)/funcs
INCLUDE = $(SRC)/include

BUILD = .build
OBJ = $(BUILD)/obj
BIN = $(BUILD)/bin

FRAMEWORK = $(SRC)/tester.c
FRAMEWORK_EXEC = $(BIN)/tester

# TODO: Add a command that builds the region.so file automatically if it's missing

$(BUILD):
	mkdir -p .build

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

build: $(OBJ)
	$(CC) -shared -o $(OBJ)/__region_alloc.so -fPIC $(FUNCS)/__region_alloc.c

run: build $(BIN)
	$(CC) $(FRAMEWORK) -o $(FRAMEWORK_EXEC) -ldl
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
