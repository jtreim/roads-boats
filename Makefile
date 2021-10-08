CC        := g++
LD        := g++

FLAGS     := -g -Wall -Wextra -pthread

MODULES   := buildings common players portables tiles windows
SRC_DIR   := $(addprefix src/,$(MODULES)) # Searches src/* for .h/.cpp files
BUILD_DIR := $(addprefix build/,$(MODULES)) # Recreates build dir to match src

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,src)

OUT       := rb.exe

vpath %.cpp $(SRC_DIR) $(TEST_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $$< -o $$@
endef

# Commands:
# make all 			 - Build everything into one executable.
# make checkdirs - Rebuild build directory to match src directory structure.
# make clean		 - Clear build directory.


.PHONY: all checkdirs clean

all: checkdirs build/rb.exe

build/rb.exe: $(OBJ)
	$(LD) -municode $^ -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

# Clean target
clean :
	@rm -rf $(BUILD_DIR)
	@rm -f build/rb.exe

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))