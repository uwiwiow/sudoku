cmake_minimum_required(VERSION 4.1)
project(sudoku-simple VERSION 1.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

file(GLOB SRC CONFIGURE_DEPENDS src/*.h src/*.c)
file(COPY assets DESTINATION .)

add_executable(sudoku-simple ${SRC})

set(BUILD_SHARED_LIBS OFF)

find_library(RAYLIB_STATIC NAMES raylib PATHS /usr/local/lib NO_DEFAULT_PATH)
if(RAYLIB_STATIC)
    message(STATUS "Using static Raylib: ${RAYLIB_STATIC}")
    set(RAYLIB ${RAYLIB_STATIC})
else()
    message(WARNING "Static libraylib.a not found, falling back to shared version")
    find_package(raylib REQUIRED)
    set(RAYLIB raylib)
endif()

target_link_libraries(sudoku-simple ${RAYLIB} m)

target_compile_definitions(sudoku-simple PRIVATE
        SUDOKU_VERSION="${PROJECT_VERSION}"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/Sudoku-simple)
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    add_custom_command(TARGET sudoku-simple POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sudoku-simple> ${OUTPUT_DIR}/
    )
    add_custom_command(TARGET sudoku-simple POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets ${OUTPUT_DIR}/assets
    )

    string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} ARCH)

    add_custom_target(package_release ALL
            COMMAND ${CMAKE_COMMAND} -E tar "cfv"
            ${CMAKE_BINARY_DIR}/sudoku-simple-${PROJECT_VERSION}-${ARCH}.zip
            --format=zip Sudoku-simple
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS sudoku-simple
    )
endif()