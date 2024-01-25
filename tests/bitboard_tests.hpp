#pragma once
#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <utility>
#include "reversi.h"



TEST(BitboardTest, revCountFirstZeros) {
    std::vector<std::pair<int, RevBitboard>> cases = {
        { 64, 0 },
        { 63, 1 },
        { 62, 2 },
        { 62, 3 },
        { 61, 4 },
        { 59, 16 },
        { 59, 17 },
        { 0, 0xFFFFFFFFFFFFFFFF },
    };
    for (auto c : cases) {
        EXPECT_EQ(c.first, revCountFirstZeros(c.second));
    }
}

TEST(BitboardTest, revCountOnes) {
    std::vector<std::pair<int, RevBitboard>> cases = {
        { 0, 0 },
        { 1, 1 },
        { 1, 2 },
        { 2, 3 },
        { 1, 4 },
        { 1, 16 },
        { 2, 17 },
        { 64, 0xFFFFFFFFFFFFFFFF },
    };
    for (auto c : cases) {
        EXPECT_EQ(c.first, revCountOnes(c.second));
    }
}

TEST(BitboardTest, revXYToPos) {
    std::vector<std::array<int, 3>> cases = {
        { 0, 0, 0 },
        { 1, 1, 0 },
        { 8, 0, 1 },
        { 35, 3, 4 },
    };
    for (auto c : cases) {
        EXPECT_EQ(c[0], revXYToPos(c[1], c[2]));
    }
}

TEST(BitboardTest, revIsTrueAt) {
    std::vector<std::pair<int, RevBitboard>> cases = {
        { 0, 1 },
        { 1, 2 },
        { 2, 4 },
        { 0, 5 },
    };
    for (auto c : cases) {
        EXPECT_TRUE(revIsTrueAt(c.second, c.first));
    }
}

TEST(BitboardTest, revIsFalseAt) {
    std::vector<std::pair<int, RevBitboard>> cases = {
        { 1, 1 },
        { 0, 2 },
        { 1, 4 },
        { 1, 5 },
    };
    for (auto c : cases) {
        EXPECT_FALSE(revIsTrueAt(c.second, c.first));
    }
}

TEST(BitboardTest, revIsTrueAtXY) {
    std::vector<std::array<int, 3>> cases = {
        { 0, 0, 1 },
        { 1, 0, 2 },
        { 2, 1, 4 << 8 },
        { 0, 2, 5 << 16},
    };
    for (auto c : cases) {
        EXPECT_TRUE(revIsTrueAtXY(c[2], c[0], c[1]));
    }
}

TEST(BitboardTest, revIsFalseAtXY) {
    std::vector<std::array<int, 3>> cases = {
        { 0, 1, 1 },
        { 1, 1, 2 },
        { 2, 0, 4 << 8 },
        { 0, 1, 5 << 16 },
    };
    for (auto c : cases) {
        EXPECT_FALSE(revIsTrueAtXY(c[2], c[0], c[1]));
    }
}

TEST(BitboardTest, revBitboardToArray) {
    std::vector<std::pair<RevBitboard, std::vector<int>>> cases = {
        { 0, {} },
        { 1, { 0 } },
        { 2, { 1 } },
        { 3, { 0, 1 } },
        { 4, { 2 } },
        { 16, { 4 } },
        { 19, { 0, 1, 4 } },
        { 0x8000000000000000, { 63 } },
        { 0xF000000000000000, { 60, 61, 62, 63 } },
    };

    for (auto c : cases) {
        RevBitboard b = c.first;
        int *array = revBitboardToArray(b);
        int size = revCountOnes(b);
        std::vector<int> expected = c.second;
        EXPECT_EQ(expected.size(), size);
        for (int i = 0; i < size; i++) {
            EXPECT_EQ(expected[i], array[i]);
        }
        free(array);
    }
}

TEST(BitboardTest, revArrayToBitboard) {
    std::vector<std::pair<RevBitboard, std::vector<int>>> cases = {
        { 0, {} },
        { 1, { 0 } },
        { 2, { 1 } },
        { 3, { 0, 1 } },
        { 4, { 2 } },
        { 16, { 4 } },
        { 19, { 0, 1, 4 } },
        { 0x8000000000000000, { 63 } },
        { 0xF000000000000000, { 60, 61, 62, 63 } },
    };

    for (auto c : cases) {
        RevBitboard expected = c.first;
        int* ptr = NULL;
        if (c.second.size() != 0) {
            ptr = &c.second[0];
        }
        RevBitboard b = revArrayToBitboard(ptr, c.second.size());
        EXPECT_EQ(expected, b);
    }
}
