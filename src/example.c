#include <stdio.h>
#include <time.h>
#include "reversi.h"

int inputInt(const char* msg) {
    int x = -1;
    while ((x < 0) | (x >= 8)) {
        printf("%s", msg);
        scanf("%d", &x);
        fflush(stdin);
    }
    return x;
}

int main() {
    // Initialize
    RevBoard *board = revNewBoard();
    if (board == NULL) {
        printf("Failed to create a reversi board.\n");
        return 1;
    }
    revInitGenRandom((unsigned)time(NULL));

    // Iterate until no one can put disks.
    while (revHasLegalMoves(board)) {
        int move;
        if (revGetCurrentPlayer(board) == DISK_BLACK) {
            printf("Your turn.\n");
            revPrintBoardWithMobility(board);
            int x = -1;
            int y = -1;
            while ((x == -1) | !revIsLegalMoveXY(board, x, y)) {
                x = inputInt("x? ");
                y = inputInt("y? ");
            }
            move = revXYToPos(x, y);
        } else {
            // CPU uses the monte carlo search.
            printf("CPU's turn.\n");
            revPrintBoardWithMobility(board);
            // Finish 20000 games and use the best move that has the highest win rate.
            move = revGenMoveMonteCarlo(board, 20000);
        }

        printf("move: %d, %d\n\n", move % 8, move / 8);
        revMove(board, move);

        if (!revHasLegalMoves(board)) {
            // Pass
            revChangePlayer(board);
        }
    }

    RevDiskType winner = revGetWinner(board);
    if (winner == DISK_BLACK) {
        printf("You win!\n");
    } else if (winner == DISK_WHITE) {
        printf("You lose.\n");
    } else {
        printf("Draw.\n");
    }

    revFreeBoard(board);
    return 0;
}
