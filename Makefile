BUILD := .build
OBJ := $(BUILD)/obj
BIN := $(BUILD)/bin

TARGET := $(BIN)/region

start:
	@echo "usage: make <command>\n\tuse 'make help' to get more commands"

help:
	@echo "\tmake run   -- builds the whole project and creates"
	@echo "\t              the executable file in the '.build/bin' directory"
	@echo "\tmake build -- creates the 'region.o' file and puts it in the '.build/obj' directory"
	@echo "\tmake clean -- removes the '.build' folder"

all: $(TARGET)

$(TARGET): $(OBJ)/region.o | $(BIN)
	gcc src/main.c $(OBJ)/region.o -o $(TARGET)

$(OBJ)/region.o: src/region.h | $(OBJ)
	gcc -std=gnu99 -DREGION_IMPLEMENTATION -x c -c src/region.h -o $(OBJ)/region.o

$(BUILD):
	mkdir -p $(BUILD)

$(OBJ): | $(BUILD)
	mkdir -p $(OBJ)

$(BIN): | $(BUILD)
	mkdir -p $(BIN)

clean:
	rm -rf $(BUILD)

build: $(OBJ)/region.o

run: $(TARGET)