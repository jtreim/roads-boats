CC        := g++
LD        := g++

FLAGS     := -g -Wall -Wextra -pthread -std=c++20

MODULES   := buildings \
						 buildings/factories \
						 buildings/producers \
						 common \
						 players \
						 portables \
						 portables/resources \
						 portables/transporters \
						 portables/transporters/land \
						 portables/transporters/sea \
						 tiles \
						 tiles/components \
						 utils \
						 windows
SRC_DIR   := $(addprefix src/,$(MODULES))   # Searches src/* for .h/.cpp files
BUILD_DIR := $(addprefix build/src/,$(MODULES)) # Recreates build dir to match src
TEST_DIR  := tests

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/src/%.o,$(SRC))
INCLUDES  := $(addprefix -I,src)
LIBS      := $(addprefix -I,libs)

OUT       := rb.exe

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDES) $(LIBS) -c $$< -o $$@
endef

# Commands:
# make all       - Build everything into one executable.
# make checkdirs - Rebuild build directory to match src directory structure.
# make clean     - Clear build directory.
# make test      - Build and run unit tests.


.PHONY: all checkdirs clean test

all: checkdirs build/rb.exe

build/rb.exe: $(OBJ)
	$(LD) -municode $^ -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

test:
	$(MAKE) -C tests

# Clean target
clean :
	@rm -rf $(BUILD_DIR)
	@rm -f build/rb.exe
	@rm -rf tests/build/*.o tests/build/*.a
	

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))