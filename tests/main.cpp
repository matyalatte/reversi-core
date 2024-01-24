// Tests for main_frame.cpp
// Todo: Write more tests

#include <gtest/gtest.h>
#include "reversi.h"
#include "bitboard_tests.hpp"
#include "reversi_tests.hpp"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
