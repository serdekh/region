CC = gcc
# The warnings are temporarily disabled because they treat the `region_error_code_as_strings` array
# as unused since it's only referred to in macros and not in functions
CC_FLAGS = -std=gnu99# -Wall -Wextra

# Note: all the Windows specific commands have to manually
# configured since every Linux utility has to be replaced
# with its equivalent from the MinGW environment. If this
# makefile fails to build the project because it cannot 
# find mkdir, gcc, etc., then the path to the executable
# should be explicitly configured in the following if-statement

ifeq ($(OS),Windows_NT)
	UTILS_BIN_PATH = \MinGW\msys\1.0\bin
	
	EXT_EXEC = exe
	EXT_OBJ  = o
	EXT_SOBJ = dll
	EMPTY= 
	TAB_CHAR = $(EMPTY)    
	
	TARGET_OBJ_FLAGS = -DREGION_BUILD
else
	UTILS_BIN_PATH =

	EXT_EXEC =
	EXT_OBJ  = o
	EXT_SOBJ = so
	TAB_CHAR = \t

	TARGET_OBJ_FLAGS =
endif

MKDIR = $(UTILS_BIN_PATH)\mkdir.$(EXT_EXEC)
RM    = $(UTILS_BIN_PATH)\rm.$(EXT_EXEC)
MAKE = $(UTILS_BIN_PATH)\make.$(EXT_EXEC)

BUILD := .build
SRC := src
OBJ := $(BUILD)/obj
SOBJ := $(BUILD)/sobj
BIN := $(BUILD)/bin

TESTS := tests
TESTS_BUILD := $(TESTS)/.build

TARGET_TAG := region
TARGET_FILE := $(SRC)/$(TARGET_TAG).h
TARGET_TEST_TAG := $(TARGET_TAG)-test
TARGET_BIN := $(BIN)/$(TARGET_TAG)
TARGET_OBJ := $(OBJ)/$(TARGET_TAG).$(EXT_OBJ)
TARGET_SOBJ := $(SOBJ)/$(TARGET_TAG).$(EXT_SOBJ)
TARGET_TEST_SOBJ := $(SOBJ)/$(TARGET_TEST_TAG).$(EXT_SOBJ)

usage:
	@echo "usage: make <command>$(TAB_CHAR)"
	@echo "$(TAB_CHAR)use 'make help' to get more commands"

$(BUILD):
	$(MKDIR) -p $(BUILD)

$(OBJ): $(BUILD)
	$(MKDIR) -p $(OBJ)

$(BIN): $(BUILD)
	$(MKDIR) -p $(BIN)

$(SOBJ): $(BUILD)
	$(MKDIR) -p $(SOBJ)

$(TESTS):
	$(MKDIR) -p $(TESTS)

$(TARGET_BIN): $(TARGET_OBJ) $(BIN)
	$(CC) $(CC_FLAGS) $(TARGET_OBJ_FLAGS) $(TARGET_OBJ) $(SRC)/main.c -o $(TARGET_BIN)

$(TARGET_OBJ): $(TARGET_FILE) $(OBJ)
	$(CC) $(CC_FLAGS) $(TARGET_OBJ_FLAGS) -x c -c $(TARGET_FILE) -o $(TARGET_OBJ)

$(TARGET_SOBJ): $(TARGET_FILE) $(SOBJ)
	$(CC) $(CC_FLAGS) $(TARGET_OBJ_FLAGS) -x c -c $(TARGET_FILE) -o $(TARGET_SOBJ)
	$(CC) -shared -o $(TARGET_SOBJ) -x c -fPIC $(TARGET_FILE)

help:
	@echo "$(TAB_CHAR)make run               -- Builds the whole project and runs it"
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)Requires a 'main.c' file in the '<project>/src/' subdirectory"
	@echo "$(TAB_CHAR)make build             -- Builds the region library and produces an object file"
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)Tests are not built, the object file is located at '<project>/.build/obj'"
	@echo "$(TAB_CHAR)make build-shared      -- Builds the region library and produces a shared object file"   
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)Tests are not built, the object file is located at '<project>/.build/sobj'"
	@echo "$(TAB_CHAR)make test              -- Builds a testing system and the region shared object for tests."
	@echo "$(TAB_CHAR)                          Calls the 'run' command from the inner Makefile. Runs upon success"
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)Tests are built, the files are located at       "
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)$(TAB_CHAR)'<project>/.build'           - region shared object (with the REGION_TEST_IMPLEMENTATION macro defined)"
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)$(TAB_CHAR)'<project>/tests/.build/obj' - object files with tests  "
	@echo "$(TAB_CHAR)                          $(TAB_CHAR)$(TAB_CHAR)'<project>/tests/.build/bin' - test framework executable"
	

clean:
	$(RM) -rf $(BUILD) $(TESTS_BUILD)

build: $(TARGET_OBJ)

build-shared: $(TARGET_SOBJ)

build-shared-test: $(TARGET_TEST_SOBJ)

run: $(TARGET_BIN) $(TARGET_OBJ)
	./$(TARGET_BIN)

test:
	$(MAKE) -C tests run