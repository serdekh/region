usage:
	@echo "usage: make <command>\n\tuse 'make help' to get more commands"

#----------------------- Folders ----------------------------

# The folder in which all binaries and objects files,
# that are related to the region, go into.
# It chould contain two subdirectories:
#	<obj-dir>: for all object files (.o|.so)
#	<bin-dir>: for all executables
# If any of these subdirectories are missing, 
# an error may occur.
BUILD := .build
$(BUILD):
	mkdir -p $(BUILD)

# The folder in which all objects files, that are related
# to the region, go into. It sits inside the $(BUILD) 
# folder and it is the target path for the region.o file.
OBJ := $(BUILD)/obj
$(OBJ): $(BUILD)
	mkdir -p $(OBJ)

# The folder in which all binaries, that are related
# to the region, go into. It sits inside the $(BUILD) 
# folder and it is the target path for the 'region'
# executable file. Note that an executable is only
# created if the source tree contains the main.c file.
# Otherwise a compilation error occurs.
BIN := $(BUILD)/bin
$(BIN): $(BUILD)
	mkdir -p $(BIN)

#  Testing is separated from the region. The library 
# can be compiled without testing. But for the testing 
# binary, the object file of the library is required which
# is automatically generated if it's missing.
# If some of these conditions are invalid, an 
# error occurs and the testing process terminates. 
TESTS := tests
$(TESTS):
	mkdir -p $(TESTS)

TESTS_BUILD := $(TESTS)/.build

# -------------------- Files --------------------

# The library executable (requires a src/main.c)
TARGET_BIN := $(BIN)/region
# The static library object used in tests 
TARGET_OBJ := $(OBJ)/region.o

# Builds the library executable statically 
# linking with the object file and puts it
# in the binary subdirectory. 
$(TARGET_BIN): $(TARGET_OBJ) $(BIN)
	gcc src/main.c $(TARGET_OBJ) -o $(TARGET_BIN)

# Builds the library object file.
# Depends on the main header file and the 
# object files subdirectory within the 
# build folder. Runs the compilation and
# defines the implementation so the user's
# code can just include the header. The 
$(TARGET_OBJ): src/region.h $(OBJ)
	gcc -std=gnu99 -DREGION_IMPLEMENTATION -x c -c src/region.h -o $(TARGET_OBJ)

# -------------------- Commands --------------------

# Prints the message with the additional information 
# about the commands that are supported by this makefile.
help:
	@echo "\tmake run   -- builds the whole project and creates"
	@echo "\t              the executable file inside the '.build/bin' directory"
	@echo "\tmake build -- creates the 'region.o' file and puts it in the '.build/obj' directory"
	@echo "\tmake clean -- removes the '.build' and 'tests/bin' folders"
	@echo "\tmake test  -- builds the test system and runs it upon successful build."

# Removes the $(TESTS_BIN) and $(BUILD) folders and their contents.
clean:
	rm -rf $(BUILD) $(TESTS_BUILD)

# Builds the object file of the library and puts it inside the
# $(OBJ) subdirectory. During the build, the REGION_IMPLEMENTATION 
# macro is defined so when the header is included in user's code
# no implementation macro has to be specified. Only the object file
# has to be added when the user's project is being built. 
build: $(TARGET_OBJ)

# Builds the whole project (except the tests) and runs it upon 
# successful compilation.
run: $(TARGET_BIN) $(TARGET_OBJ)
	make build
	./$(TARGET_BIN)

# Builds the tests and the object file if it's missing.
# The object file is the one being tested. Upon the
# successful compilation, the executable is placed in the
# $(TESTS_BIN) subdirectory and it immidiately runs.
test:
	make -C tests run