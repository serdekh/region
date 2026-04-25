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

REGION_SOBJ = ../.build/sobj/region-test.so

$(BUILD):
	mkdir -p .build

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

$(REGION_SOBJ):
	make -C .. build-shared-test

TARGETS_SRCS := $(shell find $(TARGETS) -name '*.c')

$(OBJ)/%.so: $(TARGETS)/%.c $(OBJ)
	$(CC) -shared -fPIC -o $@ $<

TARGETS_SOS := $(patsubst $(TARGETS)/%.c,$(OBJ)/%.so,$(TARGETS_SRCS))

$(FRAMEWORK_EXEC): $(BUILD) $(OBJ) $(BIN)
	$(CC) $(FRAMEWORK_C) $(FRAMEWORK_PATH)/main.c -o $(FRAMEWORK_EXEC) -ldl

build: $(REGION_SOBJ) $(OBJ) $(BIN) $(TARGETS_SOS) $(FRAMEWORK_EXEC)

run: build
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
