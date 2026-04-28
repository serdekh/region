ifeq ($(OS),Windows_NT)
	UTILS_BIN_PATH = \MinGW\msys\1.0\bin
	
	EXT_EXEC = exe
	EXT_SOBJ = dll
	
	FRAMEWORK_EXEC_FLAGS =
	TARGET_FLAGS = -shared -DREGION_BUILD
else
	UTILS_BIN_PATH =

	EXT_EXEC =
	EXT_SOBJ = so

	FRAMEWORK_EXEC_FLAGS = -ldl
	TARGET_FLAGS = -shared -fPIC
endif

MKDIR = $(UTILS_BIN_PATH)\mkdir.$(EXT_EXEC)
RM    = $(UTILS_BIN_PATH)\rm.$(EXT_EXEC)
MAKE = $(UTILS_BIN_PATH)\make.$(EXT_EXEC)

CC = gcc

SRC       = src
TARGETS   = $(SRC)/targets
INCLUDE   = $(SRC)/include

BUILD = .build
OBJ   = $(BUILD)/obj
BIN   = $(BUILD)/bin

FRAMEWORK_TAG = rt
FRAMEWORK_PATH = $(SRC)/framework
FRAMEWORK_C    = $(FRAMEWORK_PATH)/$(FRAMEWORK_TAG).c
FRAMEWORK_EXEC = $(BIN)/$(FRAMEWORK_TAG)-exec

REGION_SOBJ = ../.build/sobj/region-test.$(EXT_SOBJ)

$(BUILD):
	$(MKDIR) -p .build

$(OBJ): $(BUILD)
	$(MKDIR) -p $(OBJ)

$(BIN): $(BUILD)
	$(MKDIR) -p $(BIN)

$(REGION_SOBJ):
	$(MAKE) -C .. build-shared-test

TARGETS_SRCS := $(wildcard $(TARGETS)/*.c)

$(OBJ)/%.$(EXT_SOBJ): $(TARGETS)/%.c $(OBJ)
	$(CC) $(TARGET_FLAGS) -o $@ $<

TARGETS_SOS := $(patsubst $(TARGETS)/%.c,$(OBJ)/%.$(EXT_SOBJ),$(TARGETS_SRCS))

$(FRAMEWORK_EXEC): $(BUILD) $(OBJ) $(BIN)
	$(CC) $(FRAMEWORK_C) $(FRAMEWORK_EXEC_FLAGS) $(FRAMEWORK_PATH)/main.c -o $(FRAMEWORK_EXEC)

build: $(REGION_SOBJ) $(OBJ) $(BIN) $(TARGETS_SOS) $(FRAMEWORK_EXEC)

run: build
	./$(FRAMEWORK_EXEC)

clean:
	$(RM) -rf $(BUILD)
