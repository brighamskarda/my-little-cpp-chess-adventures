cmake_minimum_required(VERSION 4.1)
project(better_cpp)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

option(BETTER_CPP_BUILD_TESTS "Build tests for better_cpp" ${PROJECT_IS_TOP_LEVEL})

add_library(better_cpp STATIC src/text.cpp)
target_include_directories(better_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (BETTER_CPP_BUILD_TESTS)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif ()

if (MSVC)
    target_compile_options(chess_again PRIVATE /W4 /permissive-)
else ()
    target_compile_options(chess_again PRIVATE -Wall -Wextra -pedantic)
endif ()