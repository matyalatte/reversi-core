#pragma once
#include <time.h>
#include <gtest/gtest.h>
#include <vector>
#include "reversi.h"

class ReversiTest : public ::testing::Test {
 protected:
    RevBoard* board;

    virtual void SetUp() {
        board = revNewBoard();
        ASSERT_TRUE(board != NULL);
    }

    virtual void TearDown() {
        revFreeBoard(board);
    }

    void compareIntArray(const std::vector<int> &expected, int *actual, int size) {
        EXPECT_EQ(expected.size(), size);
        for (int i = 0; i < size; i++) {
            EXPECT_EQ(expected[i], actual[i]);
        }
    }
};

TEST(ReversiUtilTest, revGetVersion) {
    printf("%s\n", revGetVersion());
}

TEST(ReversiUtilTest, revGetVersionAsInt) {
    printf("%d\n", revGetVersionAsInt());
}

TEST_F(ReversiTest, revNewBoardTest) {
    EXPECT_EQ(2, revCountDisks(board, DISK_BLACK));
    EXPECT_EQ(2, revCountDisks(board, DISK_WHITE));
}

TEST_F(ReversiTest, revInitBoardTest) {
    revMoveXY(board, 3, 2);
    EXPECT_EQ(4, revCountDisks(board, DISK_BLACK));
    EXPECT_EQ(1, revCountDisks(board, DISK_WHITE));
    revInitBoard(board);
    EXPECT_EQ(2, revCountDisks(board, DISK_BLACK));
    EXPECT_EQ(2, revCountDisks(board, DISK_WHITE));
}

int areSameBoards(RevBoard *b1, RevBoard *b2) {
    return (revGetBitboard(b1, DISK_BLACK) == revGetBitboard(b2, DISK_BLACK)) &&
           (revGetBitboard(b1, DISK_WHITE) == revGetBitboard(b2, DISK_WHITE)) &&
           (revGetMobility(b1) == revGetMobility(b2)) &&
           (revGetMobilityCount(b1) == revGetMobilityCount(b2)) &&
           (revGetCurrentPlayer(b1) == revGetCurrentPlayer(b2));
}

TEST_F(ReversiTest, revCopyBoardTest) {
    RevBoard *board2 = revNewBoard();
    revSetBitboard(board2, DISK_BLACK, 0xFFFFFFFF);
    EXPECT_FALSE(areSameBoards(board, board2));
    revCopyBoard(board, board2);
    EXPECT_TRUE(areSameBoards(board, board2));
    revFreeBoard(board2);
}

TEST_F(ReversiTest, revGetCurrentPlayer) {
    EXPECT_EQ(DISK_BLACK, revGetCurrentPlayer(board));
    revChangePlayer(board);
    EXPECT_EQ(DISK_WHITE, revGetCurrentPlayer(board));
    revChangePlayer(board);
    EXPECT_EQ(DISK_BLACK, revGetCurrentPlayer(board));
}

TEST_F(ReversiTest, revGetBitboardAsArray) {
    int* disks = revGetBitboardAsArray(board, DISK_BLACK);
    int count = revCountDisks(board, DISK_BLACK);
    std::vector<int> expected = {
        revXYToPos(4, 3),
        revXYToPos(3, 4),
    };
    compareIntArray(expected, disks, count);
    free(disks);

    disks = revGetBitboardAsArray(board, DISK_WHITE);
    count = revCountDisks(board, DISK_WHITE);
    expected = {
        revXYToPos(3, 3),
        revXYToPos(4, 4),
    };
    compareIntArray(expected, disks, count);
    free(disks);
}

TEST_F(ReversiTest, revGetDiskXY) {
    RevDiskType type = revGetDiskXY(board, 3, 4);
    EXPECT_EQ(DISK_BLACK, type);
    type = revGetDiskXY(board, 3, 3);
    EXPECT_EQ(DISK_WHITE, type);
    type = revGetDiskXY(board, 2, 2);
    EXPECT_EQ(DISK_NONE, type);
}

TEST_F(ReversiTest, revSetDiskXY) {
    RevDiskType type = revGetDiskXY(board, 3, 4);
    EXPECT_EQ(DISK_BLACK, type);
    revSetDiskXY(board, DISK_WHITE, 3, 4);
    type = revGetDiskXY(board, 3, 4);
    EXPECT_EQ(DISK_WHITE, type);
    EXPECT_EQ(1, revCountDisks(board, DISK_BLACK));
    EXPECT_EQ(3, revCountDisks(board, DISK_WHITE));
    revSetDiskXY(board, DISK_BLACK, 3, 4);
    type = revGetDiskXY(board, 3, 4);
    EXPECT_EQ(DISK_BLACK, type);
    EXPECT_EQ(2, revCountDisks(board, DISK_BLACK));
    EXPECT_EQ(2, revCountDisks(board, DISK_WHITE));
}

TEST_F(ReversiTest, revSetDiskNone) {
    RevDiskType type = revGetDiskXY(board, 3, 4);
    EXPECT_EQ(DISK_BLACK, type);
    revSetDiskXY(board, DISK_NONE, 3, 4);
    type = revGetDiskXY(board, 3, 4);
    EXPECT_EQ(DISK_NONE, type);
    EXPECT_EQ(1, revCountDisks(board, DISK_BLACK));
    EXPECT_EQ(2, revCountDisks(board, DISK_WHITE));
}

TEST_F(ReversiTest, revGetMobility) {
    int *array = revBitboardToArray(revGetMobility(board));
    int size = revGetMobilityCount(board);
    std::vector<int> expected = {
        revXYToPos(3, 2),
        revXYToPos(2, 3),
        revXYToPos(5, 4),
        revXYToPos(4, 5),
    };
    compareIntArray(expected, array, size);
    free(array);

    revMoveXY(board, 3, 2);

    array = revBitboardToArray(revGetMobility(board));
    size = revGetMobilityCount(board);
    expected = {
        revXYToPos(2, 2),
        revXYToPos(4, 2),
        revXYToPos(2, 4),
    };
    compareIntArray(expected, array, size);
    free(array);
}

TEST_F(ReversiTest, revGetMobility2) {
    // Test if a case that had wrong mobility was fixed or not.
    std::vector<int> ba = {
        revXYToPos(0, 0),
        revXYToPos(1, 0),
        revXYToPos(3, 0),
        revXYToPos(0, 1),
        revXYToPos(1, 1),
        revXYToPos(2, 1),
        revXYToPos(3, 1),
        revXYToPos(1, 2),
        revXYToPos(3, 2),
        revXYToPos(2, 3),
        revXYToPos(3, 3),
        revXYToPos(3, 4),
    };
    std::vector<int> wa = {
        revXYToPos(4, 1),
        revXYToPos(2, 2),
        revXYToPos(4, 2),
        revXYToPos(1, 3),
        revXYToPos(4, 3),
        revXYToPos(5, 3),
        revXYToPos(4, 4),
    };
    RevBitboard black = revArrayToBitboard(&ba[0], ba.size());
    RevBitboard white = revArrayToBitboard(&wa[0], wa.size());
    revSetBitboard(board, DISK_BLACK, black);
    revSetBitboard(board, DISK_WHITE, white);
    revChangePlayer(board);
    revPrintBoardWithMobility(board);
    std::vector<int> expected = {
        revXYToPos(2, 0),
        revXYToPos(4, 0),
        revXYToPos(0, 2),
        revXYToPos(1, 4),
        revXYToPos(2, 4),
        revXYToPos(2, 5),
    };
    int *array = revGetMobilityAsArray(board);
    compareIntArray(expected, array, revGetMobilityCount(board));
    free(array);
}

TEST_F(ReversiTest, revGetMobility3) {
    // Test if a case that had wrong mobility was fixed or not.
    std::vector<int> moves = {
        3, 2,
        2, 2,
        2, 3,
        2, 4,
        3, 5,
        4, 6,
        1, 5,
        1, 4,
        2, 5,
        2, 6,
        0, 5,
        5, 2,
        2, 1,
        2, 0,
        4, 5,
        5, 6,
        2, 7,
        1, 6,
        5, 4,
        3, 6,
        0, 7,
        0, 3,
        0, 4,
        4, 1,
        1, 7,
        5, 3,
        3, 7,
        1, 3,
        4, 7,
        5, 5,
        6, 5,
        6, 6,
        4, 2,
        5, 7,
        7, 7,
        5, 1,
        6, 7,
        6, 4,
        4, 0,
        5, 0,
        6, 0,
        3, 1,
        0, 2,
        0, 6,
        1, 2,
        7, 6,
        7, 5,
        7, 4,
        7, 3,
        6, 3,
        7, 2,
        0, 1,
    };
    int move_count = moves.size() / 2;
    for (int i = 0; i < move_count; i++) {
        revMoveXY(board, moves[i * 2], moves[i * 2 + 1]);
    }
    revPrintBoardWithMobility(board);

    std::vector<int> expected = {
        revXYToPos(0, 0),
        revXYToPos(1, 0),
        revXYToPos(3, 0),
        revXYToPos(1, 1),
        revXYToPos(6, 1),
        revXYToPos(6, 2),
    };
    int *array = revGetMobilityAsArray(board);
    compareIntArray(expected, array, revGetMobilityCount(board));
    free(array);
}

static int inArray(int elm, int *array, int size) {
    int found = 0;
    int *ap = array;
    for (; ap < array + size; ap++) {
        found |= (ap[0] == elm);
    }
    return found;
}

TEST_F(ReversiTest, revIsLegalMoveXY) {
    EXPECT_TRUE(revIsLegalMoveXY(board, 3, 2));
    EXPECT_TRUE(revIsLegalMoveXY(board, 2, 3));
    EXPECT_TRUE(revIsLegalMoveXY(board, 5, 4));
    EXPECT_TRUE(revIsLegalMoveXY(board, 4, 5));
    EXPECT_FALSE(revIsLegalMoveXY(board, 2, 2));
    EXPECT_FALSE(revIsLegalMoveXY(board, 0, 7));
}

TEST_F(ReversiTest, revPrintBoard) {
    // Just test if it will raise errors or not.
    revPrintBoard(board);
}

TEST_F(ReversiTest, revGenMoveRandom) {
    revInitGenRandom((unsigned)time(NULL));

    std::array<int, 4> expected_moves = {
        revXYToPos(3, 2),
        revXYToPos(2, 3),
        revXYToPos(5, 4),
        revXYToPos(4, 5),
    };
    std::array<int, 4> move_counts = { 0, 0, 0, 0 };
    for (int i = 0; i < 10000; i++) {
        int move = revGenMoveRandom(board);
        EXPECT_TRUE(inArray(move, &expected_moves[0], 4));
        for (int j = 0; j < 4; j++) {
            if (move == expected_moves[j])
                move_counts[j]++;
        }
    }
    for (int i : move_counts) {
        // Expects revGenMoveRandom() picks each move more than 2200 times.
        EXPECT_GT(i, 2200);
    }
}

TEST_F(ReversiTest, revGenMoveMonteCarlo) {
    // Test if revGenMoveMonteCarlo is stronger than revGenMoveRandom
    int monte_win = 0;
    int random_win = 0;
    for (int i = 0; i < 10; i++) {
        revInitBoard(board);
        while (revHasLegalMoves(board)) {
            int move;
            if (revGetCurrentPlayer(board) == DISK_BLACK) {
                move = revGenMoveMonteCarlo(board, 500);
            } else {
                move = revGenMoveRandom(board);
            }
            revMove(board, move);
            if (!revHasLegalMoves(board)) {
                revChangePlayer(board);
            }
        }
        int winner = revGetWinner(board);
        if (winner == DISK_BLACK) {
            monte_win++;
        } else if (winner == DISK_WHITE) {
            random_win++;
        }
    }
    printf("Monte Carlo Search VS Random Move: W%d, L%d, D%d\n",
           monte_win, random_win, 10 - monte_win - random_win);
    EXPECT_GT(monte_win, random_win);
}
