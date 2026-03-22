project(tests)

if (NOT TARGET gtest)
    add_subdirectory(googletest)
endif ()
add_executable(better_cpp_tests text_test.cpp)

target_link_libraries(better_cpp_tests PRIVATE gtest gtest_main better_cpp)
