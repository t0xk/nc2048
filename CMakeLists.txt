cmake_minimum_required(VERSION 3.10)
project(nc2048 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-lncurses")


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(nc2048 src/main.c  src/global.h src/field.c src/field.h src/random.c src/random.h src/score.c src/score.h)
target_link_libraries(nc2048 ${CURSES_LIBRARIES})
