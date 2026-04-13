CC = gcc

SRC = src
FUNCS = $(SRC)/funcs
INCLUDE = $(SRC)/include
TESTER = $(SRC)/tester

BUILD = .build
OBJ = $(BUILD)/obj
BIN = $(BUILD)/bin

COMMON = common
COMMON_FOLDER = $(FUNCS)/$(COMMON)
COMMON_C = $(COMMON_FOLDER)/$(COMMON).c
COMMON_OBJ_FOLDER = $(OBJ)/$(COMMON)
COMMON_SOBJ = $(COMMON_OBJ_FOLDER)/$(COMMON).so

FRAMEWORK = tester
FRAMEWORK_C = $(TESTER)/$(FRAMEWORK).c
FRAMEWORK_H = $(TESTER)/$(FRAMEWORK).c
FRAMEWORK_EXEC = $(BIN)/$(FRAMEWORK)

REGION_SOBJ = ../.build/obj/region-test.so

$(BUILD):
	mkdir -p .build

$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

$(BIN): $(BUILD)
	mkdir -p $(BIN)

$(REGION_SOBJ):
	make -C .. build-shared-test

FUNCS_SRCS := $(shell find $(FUNCS) -name '*.c')
FUNCS_SRCS := $(filter-out $(COMMON_C),$(FUNCS_SRCS))

$(COMMON_OBJ_FOLDER):
	mkdir -p $(COMMON_OBJ_FOLDER)

$(COMMON_SOBJ): $(COMMON_C) $(COMMON_OBJ_FOLDER)
	$(CC) -shared -fPIC -o $@ $<

$(OBJ)/%.so: $(FUNCS)/%.c $(COMMON_SOBJ) $(OBJ)
	$(CC) -shared -fPIC -o $@ $< $(COMMON_SOBJ)

FUNCS_SOS := $(patsubst $(FUNCS)/%.c,$(OBJ)/%.so,$(FUNCS_SRCS))

$(FRAMEWORK_EXEC): $(BUILD) $(OBJ) $(BIN) $(FUNCS_SOS)
	$(CC) $(FRAMEWORK_C) $(SRC)/main.c -o $(FRAMEWORK_EXEC) -ldl

build: $(REGION_SOBJ) $(OBJ) $(BIN) $(COMMON_SOBJ) $(FUNCS_SOS) $(FRAMEWORK_EXEC)

run: $(FRAMEWORK_EXEC) $(REGION_SOBJ)
	./$(FRAMEWORK_EXEC)

clean:
	rm -rf $(BUILD)
