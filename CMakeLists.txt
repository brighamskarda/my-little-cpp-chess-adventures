cmake_minimum_required(VERSION 4.1)
project(chess_again)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

option(CHESS_AGAIN_BUILD_TESTS "Build tests for chess_again" ${PROJECT_IS_TOP_LEVEL})
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

if (supported)
    message(STATUS "LTO (IPO) is supported and enabled.")
    # Enable it globally for all targets
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(WARNING "LTO (IPO) is not supported: ${error}")
endif ()

add_library(chess_again STATIC
        src/piece.cpp
        src/bitboard.cpp
        src/board.cpp
        src/position.cpp
        src/movegen.cpp
)

if (CHESS_AGAIN_BUILD_TESTS)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
endif ()
add_subdirectory(better_cpp)

target_link_libraries(chess_again PRIVATE better_cpp)

target_include_directories(chess_again PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CHESS_AGAIN_BUILD_TESTS)
    add_subdirectory(tests)
endif ()

if (MSVC)
    target_compile_options(chess_again PRIVATE /W4 /permissive-)
else ()
    target_compile_options(chess_again PRIVATE -Wall -Wextra -pedantic)
endif ()