cmake_minimum_required(VERSION 3.11-3.18)
project(roads-boats VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

# Add subdirectories
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.h src/*.cpp)

add_library(rb ${sources})
target_include_directories(rb PUBLIC include PRIVATE src)

set(MAIN src/game.cpp)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# create executable
add_executable(${PROJECT_NAME} ${MAIN})
