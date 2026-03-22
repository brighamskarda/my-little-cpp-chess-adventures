project(chess_again_tests)

add_executable(chess_again_tests
        square_test.cpp
        piece_test.cpp
        move_test.cpp
        board_test.cpp
        position_test.cpp
        movegen_test.cpp)

target_link_libraries(chess_again_tests PRIVATE gtest gtest_main chess_again)
