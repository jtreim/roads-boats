cmake_minimum_required(VERSION 3.0.0)
project(roads-boats VERSION 0.1.0)

# includes
include(CPack)
include(CTest)
enable_testing()

# define env
set(SOURCE src/game.cpp)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# create executable
add_executable(${PROJECT_NAME} ${SOURCE})
