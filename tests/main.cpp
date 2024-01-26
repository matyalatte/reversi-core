#include <time.h>
#include <gtest/gtest.h>
#include "reversi.h"
#include "bitboard_tests.hpp"
#include "reversi_tests.hpp"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    revInitGenRandom((unsigned)time(NULL));
    return RUN_ALL_TESTS();
}
