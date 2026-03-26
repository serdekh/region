usage:
	@echo "usage: make <command>\n\tuse 'make help' to get more commands"

CC = gcc
# The warnings are temporarily disabled because they treat the `region_error_code_as_strings` array
# as unused since it's only referred to in macros and not in functions
CC_FLAGS = -std=gnu99 #-Wall -Wextra

BUILD := .build
OBJ := $(BUILD)/obj
SOBJ := $(BUILD)/obj
BIN := $(BUILD)/bin

TESTS := tests
TESTS_BUILD := $(TESTS)/.build

TARGET_TAG := region
TARGET_BIN := $(BIN)/$(TARGET_TAG)
TARGET_OBJ := $(OBJ)/$(TARGET_TAG).o
TARGET_SOBJ := $(SOBJ)/$(TARGET_TAG).so

$(BUILD):
	mkdir -p $(BUILD)

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

$(SOBJ): $(BUILD)
	mkdir -p $(SOBJ)

$(TESTS):
	mkdir -p $(TESTS)

$(TARGET_BIN): $(TARGET_OBJ) $(BIN)
	$(CC) $(CC_FLAGS) src/main.c $(TARGET_OBJ) -o $(TARGET_BIN)

$(TARGET_OBJ): src/region.h $(OBJ)
	$(CC) $(CC_FLAGS) -DREGION_IMPLEMENTATION -x c -c src/region.h -o $(TARGET_OBJ)

$(TARGET_SOBJ): src/region.h $(SOBJ)
	$(CC) $(CC_FLAGS) -DREGION_IMPLEMENTATION -x c -c src/region.h -o $(TARGET_SOBJ)
	$(CC) -shared -o $(TARGET_SOBJ) -DREGION_IMPLEMENTATION -x c -fPIC src/region.h

help:
	@echo "\tmake run   -- Builds the whole project"
	@echo "\t              1. Creates the '.build' folder"
	@echo "\t              2. Creates the '.build/bin' and '.build/obj' folders"
	@echo "\t              3. Compiles the source code header file 'src/region.h' to an object file"
	@echo "\t                 and places it in the '.build/obj' directory."
	@echo "\t              4. Compiles the 'src/main.c' file in the '.build/bin' directory"
	@echo "\t                 and links it with the '.build/obj/region.o' file"
	@echo "\t              5. Runs the '.build/bin/region' file upon successful compilation"
	@echo "\tmake build -- creates the 'region.o' file and puts it in the '.build/obj' directory"
	@echo "\tmake clean -- removes the '.build' and 'tests/bin' folders"
	@echo "\tmake test  -- builds the test system and runs it upon successful build."

clean:
	rm -rf $(BUILD) $(TESTS_BUILD)

build: $(TARGET_OBJ)

build-shared: $(TARGET_SOBJ)

run: $(TARGET_BIN) $(TARGET_OBJ)
	./$(TARGET_BIN)

test:
	make -C tests-experimental run