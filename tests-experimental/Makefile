CC = gcc

SRC = src
FUNCS = $(SRC)/funcs
INCLUDE = $(SRC)/include

BUILD = .build
OBJ = $(BUILD)/obj
BIN = $(BUILD)/bin

FRAMEWORK = $(SRC)/tester.c
FRAMEWORK_EXEC = $(BIN)/tester

$(BUILD):
	mkdir -p .build

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

build: $(OBJ)
	$(CC) -shared -o $(OBJ)/add.so -fPIC $(FUNCS)/add.c
	$(CC) -shared -o $(OBJ)/sub.so -fPIC $(FUNCS)/sub.c

run: build $(BIN)
	$(CC) $(FRAMEWORK) -o $(FRAMEWORK_EXEC) -ldl
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
