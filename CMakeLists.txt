cmake_minimum_required(VERSION 4.1)
project(sudoku C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(raylib REQUIRED)

file(GLOB SRC CONFIGURE_DEPENDS src/*.h src/*.c)
file(COPY assets DESTINATION .)

add_executable(sudoku ${SRC})

target_link_libraries(sudoku raylib)
