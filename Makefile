usage:
	@echo "usage: make <command>\n\tuse 'make help' to get more commands"

CC = gcc
# The warnings are temporarily disabled because they treat the `region_error_code_as_strings` array
# as unused since it's only referred to in macros and not in functions
CC_FLAGS = -std=gnu99 -fvisibility=hidden # -Wall -Wextra

BUILD := .build
OBJ := $(BUILD)/obj
SOBJ := $(BUILD)/sobj
BIN := $(BUILD)/bin

TESTS := tests
TESTS_BUILD := $(TESTS)/.build

TARGET_TAG := region
TARGET_TEST_TAG := $(TARGET_TAG)-test
TARGET_BIN := $(BIN)/$(TARGET_TAG)
TARGET_OBJ := $(OBJ)/$(TARGET_TAG).o
TARGET_SOBJ := $(SOBJ)/$(TARGET_TAG).so
TARGET_TEST_SOBJ := $(SOBJ)/$(TARGET_TEST_TAG).so

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
	$(CC) $(CC_FLAGS) -DREGION_IMPLEMENTATION -DREGION_BUILD -x c -c src/region.h -o $(TARGET_SOBJ)
	$(CC) -shared -o $(TARGET_SOBJ) -DREGION_IMPLEMENTATION -x c -fPIC src/region.h

$(TARGET_TEST_SOBJ): src/region.h $(SOBJ)
	$(CC) -shared $(CC_FLAGS) -DREGION_IMPLEMENTATION -DREGION_TEST_IMPLEMENTATION -x c src/region.h -o $(TARGET_TEST_SOBJ)
	$(CC) -shared -o $(TARGET_SOBJ) -DREGION_IMPLEMENTATION -DREGION_TEST_IMPLEMENTATION -x c -fPIC src/region.h

help:
	@echo "\tmake run               -- Builds the whole project and runs it"
	@echo "\t                          \tRequires a 'main.c' file in the '<project>/src/' subdirectory"
	@echo "\tmake build             -- Builds the region library and produces an object file"
	@echo "\t                          \tTests are not built, the object file is located at '<project>/.build/obj'"
	@echo "\tmake build-shared      -- Builds the region library and produces a shared object file"   
	@echo "\t                          \tTests are not built, the object file is located at '<project>/.build/sobj'"
	@echo "\tmake build-shared-test -- Builds the region library with the -REGION_TEST_IMPLEMENTATION macro"
	@echo "\t                          and produces a shared object file"
	@echo "\t                          \tTests are not built, the object file is located at '<project>/.build/sobj'"
	@echo "\tmake test              -- Builds a testing system and the region shared object for tests."
	@echo "\t                          Calls the 'run' command from the inner Makefile. Runs upon success"
	@echo "\t                          \tTests are built, the files are located at       "
	@echo "\t                          \t\t'<project>/.build'           - region shared object (with the REGION_TEST_IMPLEMENTATION macro defined)"
	@echo "\t                          \t\t'<project>/tests/.build/obj' - object files with tests  "
	@echo "\t                          \t\t'<project>/tests/.build/bin' - test framework executable"
	

clean:
	rm -rf $(BUILD) $(TESTS_BUILD)

build: $(TARGET_OBJ)

build-shared: $(TARGET_SOBJ)

build-shared-test: $(TARGET_TEST_SOBJ)

run: $(TARGET_BIN) $(TARGET_OBJ)
	./$(TARGET_BIN)

test:
	make -C tests run