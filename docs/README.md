# reversi-core

[![Build Status, GitHub Actions](https://github.com/matyalatte/reversi-core/actions/workflows/ci.yml/badge.svg)](https://github.com/matyalatte/reversi-core/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## About

reversi-core is a simple reversi library for C.  
It's not the fastest library but fast enough for the monte carlo search.  
It's not feature rich but sufficient for demo apps of libraries.  

## Requirements

- A 64-bit processor
- [Meson](https://mesonbuild.com/) for building

## Documentation

Most of the functions are here.  
[reversi-core: RevBoard Struct Reference](https://matyalatte.github.io/reversi-core/struct_rev_board.html)  
  
Rest of them are here.  
[reversi-core: include/reversi.h File Reference](https://matyalatte.github.io/reversi-core/reversi_8h.html)

## Building

### Build Whole Project

```bash
meson setup build
meson compile -C build
meson test -C build
```

### Build Library Only

```bash
meson setup build -Dexamples=false -Dtests=false
meson compile -C build
```

### Build as Subproject

You don't need to clone the git repo if you build your project with meson.  
Save the following text as `subprojects/reversi.wrap`.  

```ini
[wrap-git]
url = https://github.com/matyalatte/reversi-core.git
depth=1

[provide]
reversi = reversi_dep
```

Then, you can use reversi-core in your meson project.

```python
reversi_dep = dependency('reversi', fallback : ['reversi', 'reversi_dep'])
executable('your_exe_name', ['your_code.cpp'], dependencies : [reversi_dep])
```

```bash
meson setup build -Dreversi:examples=false -Dreversi:tests=false
meson compile -C build
```

## Examples

### Initialization

```c
int main() {
    // Create a new board
    RevBoard *board = revNewBoard();
    if (board == NULL) {
        printf("Failed to create a reversi board.\n");
        return 1;
    }

    // Need this if you use methods that require a random number generator.
    revInitGenRandom((unsigned)time(NULL));

    // do something to play reversi

    // Free a board
    revFreeBoard(board);
}
```

### Bitboard to Array

You can get xy coordinates from bitboards.

```c
// Get black disks
int *black_disks = revGetBitboardAsArray(board, DISK_BLACK);
for (int i = 0; i < revCountDisks(board, DISK_BLACK); i++) {
    int pos = black_disks[i];
    int x = pos % 8;
    int y = pos / 8;
    printf("disk%d: %d, %d", i, x, y);
}
free(black_disks);

// Get legal moves
int *mobility = revGetMobilityAsArray(board);
for (int i = 0; i < revGetMobilityCount(board); i++) {
    int pos = mobility[i];
    int x = pos % 8;
    int y = pos / 8;
    printf("move%d: %d, %d", i, x, y);
}
free(mobility);
```

### Get a Disk

You can get a disk from a board.

```c
// Get a disk at (4, 1)
RevDiskType = revGetDiskXY(board, 4, 1);
if (RevDiskType == DISK_BLACK) {
    // Black
} else if (RevDiskType == DISK_WHITE) {
    // White
} else {
    // Empty
}
```

### Check a Move

You can check if a move is legal or not.

```c
if (revIsLegalMoveXY(board, 4, 2)) {
    // (4, 2) is a legal move.
} else {
    // Illegal!
}
```

### Execute a Move

```c
// Set a disk to (2, 3) and get flipped disks
Bitboard flipped = revMoveXY(board, 2, 3);
int *flipped_array = revBitboardToArray(flipped);
for (int i = 0; i < revCountOnes(flipped); i++) {
    int pos = flipped_array[i];
    int x = pos % 8;
    int y = pos / 8;
    printf("flipped%d: %d, %d", i, x, y);
}
free(flipped_array);
```

### Generate a Move

You can use `revGenMove*()` to generate a move.  

```c
// Pick a move randomly.
int move = revGenMoveRandom(board);
printf("move: %d, %d\n", move % 8, move / 8);
revMove(board, move);

// Play 20000 games randomly and use the best move that has the highest win rate.
move = revGenMoveMonteCarlo(board, 20000);
printf("move: %d, %d\n", move % 8, move / 8);
revMove(board, move);
```

### CLI App

Command-line app to play reversi.

```c
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

    // Iterate untill no one can put disks.
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
```
