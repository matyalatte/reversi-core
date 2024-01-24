#ifndef __REVERSI_INCLUDE_REVERSI_H__
#define __REVERSI_INCLUDE_REVERSI_H__
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _REV_EXTERN
#ifdef _WIN32
#define _REV_EXTERN __declspec(dllexport) extern
#else
#define _REV_EXTERN __attribute__((visibility("default"))) extern
#endif
#endif

#define _REV_ENUM(s) typedef unsigned int s; enum

// Version info
#define REV_VERSION "0.1.0"
#define REV_VERSION_INT 100

/**
 * Get the version of reversi-core.
 * It should be of the form `x.x.x`.
 * 
 * @returns A string that represents the version.
 */
_REV_EXTERN const char* revGetVersion();

/**
 * Get the version of reversi-core as an integer.
 * The value should be `major * 100 + minor * 100 + patch`.
 * If `revGetVersion() == "1.2.3"` then `revGetVersionAsInt() == 10203`.
 * 
 * @returns An integer that represents the version.
 */
_REV_EXTERN int revGetVersionAsInt();

/**
 * Type for a bitboard. It's actualy a 64bit uint.
 *
 * @typedef RevBitboard
 */
typedef uint64_t RevBitboard;

/**
 * Counts leading zeros of a bitboard.
 * 
 * @note This method returns 64 when the input is zero.
 * 
 * @param b RevBitboard instance
 * @returns Number of leading zeros of a bitboard.
 */
_REV_EXTERN int revCountFirstZeros(RevBitboard b);

/**
 * Counts number of positive bits in a bitboard.
 * 
 * @param b RevBitboard instance
 * @returns Number of positive bits in a bitboard.
 */
_REV_EXTERN int revCountOnes(RevBitboard b);

/**
 * Returns x + y * 8
 * 
 * @param x x-coordinate on a board.
 * @param y y-coordinate on a board.
 * @returns a position on a bitboard.
 */
_REV_EXTERN int revXYToPos(int x, int y);

/**
 * Returns a bit of a bitboard.
 * 
 * @param b RevBitboard instance
 * @param pos a position on a bitboard. pos = x + y * 8
 * @returns A bit of a bitboard.
 */
_REV_EXTERN int revIsTrueAt(RevBitboard b, int pos);

/**
 * Returns a bit of a bitboard.
 * 
 * @param b RevBitboard instance
 * @param x x-coordinate on a board.
 * @param y y-coordinate on a board.
 * @returns A bit of a bitboard.
 */
_REV_EXTERN int revIsTrueAtXY(RevBitboard b, int x, int y);

/**
 * Convert a bitboard to an array of positions.
 * 
 * @note Arrays that are returned from this method should be freed with free().
 * 
 * @param b RevBitboard instance
 * @returns An array of positions on a bitboard. The size of the array is equal to revCountOnes(b).
 */
_REV_EXTERN int* revBitboardToArray(RevBitboard b);

/**
 * Create a bitboard from an array of positions.
 * @param array An array of positions on a bitboard.
 * @param size The size of the array.
 * @returns A bitboard.
 */
_REV_EXTERN RevBitboard revArrayToBitboard(int *array, int size);

/**
 * Color of disk.
 *
 * @enum RevDiskType
 */
_REV_ENUM(RevDiskType) {
    DISK_BLACK = 0,  //!< Black disk
    DISK_WHITE,  //!< White disk
    DISK_NONE,  //!< Empty
};

/**
 * Class for a reversi board.
 *
 * @struct RevBoard
 */
typedef struct RevBoard RevBoard;

/**
 * Creates a new board.
 * 
 * @memberof RevBoard
 */
_REV_EXTERN RevBoard *revNewBoard();

/**
 * Frees the memory of a board object.
 *
 * @note Every time a board is returned from revNewBoard(), this method should be called.
 * 
 * @param board The board to free memory
 * @memberof RevBoard
 */
_REV_EXTERN void revFreeBoard(RevBoard *board);

/**
 * Initializes a board with the fixed four disks.
 * 
 * @note This method is not required. revNewBoard() calls it.
 * 
 * @param board RevBoard instance
 * @memberof RevBoard
 */
_REV_EXTERN void revInitBoard(RevBoard *board);

/**
 * Deep copy for board objects.
 * It copies a board src to another board trg.
 * 
 * @memberof RevBoard
 */
_REV_EXTERN void revCopyBoard(RevBoard *src, RevBoard *trg);

/**
 * Gets the current player.
 * 
 * @param board RevBoard instance
 * @returns #RevDiskType of the current player
 * @memberof RevBoard
 */
_REV_EXTERN RevDiskType revGetCurrentPlayer(RevBoard *board);

/**
 * Gets a bitboard of a player.
 * 
 * @param board RevBoard instance
 * @param disk_type #DISK_BLACK or #DISK_WHITE
 * @returns a bitboard of a player
 * @memberof RevBoard
 */
_REV_EXTERN RevBitboard revGetBitboard(RevBoard *board, RevDiskType disk_type);

/**
 * Sets a bitboard of a player.
 * 
 * @param board RevBoard instance
 * @param disk_type #DISK_BLACK or #DISK_WHITE
 * @param b a new bitboard
 * @memberof RevBoard
 */
_REV_EXTERN void revSetBitboard(RevBoard *board, RevDiskType disk_type, RevBitboard b);

/**
 * Gets a bitboard as an array of positions on a bitboard.
 * 
 * @note Arrays that are returned from this method should be freed with free().
 * 
 * @param board RevBoard instance
 * @param disk_type #DISK_BLACK or #DISK_WHITE
 * @returns An array of integers. The size of the array is equal to revCountDisks().
 * @memberof RevBoard
 */
_REV_EXTERN int *revGetBitboardAsArray(RevBoard *board, RevDiskType disk_type);

/**
 * Gets the number of disks on a board for a player.
 * 
 * @param board RevBoard instance
 * @param disk_type #DISK_BLACK or #DISK_WHITE
 * @returns Number of disks.
 * @memberof RevBoard
 */
_REV_EXTERN int revCountDisks(RevBoard *board, RevDiskType disk_type);

/**
 * Returns which player has more disks on a board.
 * 
 * @param board RevBoard instance
 * @returns #DISK_BLACK, #DISK_WHITE, or #DISK_NONE
 * @memberof RevBoard
 */
_REV_EXTERN RevDiskType revGetWinner(RevBoard *board);

/**
 * Sets a disk on a board.
 * 
 * @note This method does NOT flip disks.
 * 
 * @param board RevBoard instance
 * @param disk_type #DISK_BLACK or #DISK_WHITE
 * @param pos a position on a bitboard. pos = x + y * 8
 * @memberof RevBoard
 */
_REV_EXTERN void revSetDisk(RevBoard *board, RevDiskType disk_type, int pos);

/**
 * Sets a disk on a board.
 * 
 * @note This method does NOT flip disks.
 * 
 * @param board RevBoard instance
 * @param disk_type #DISK_BLACK or #DISK_WHITE
 * @param x x-coordinate on a board.
 * @param y y-coordinate on a board.
 * @memberof RevBoard
 */
_REV_EXTERN void revSetDiskXY(RevBoard *board, RevDiskType disk_type, int x, int y);

/**
 * Gets a disk on a board.
 * 
 * @param board RevBoard instance
 * @param pos a position on a bitboard. pos = x + y * 8
 * @returns #DISK_BLACK, #DISK_WHITE or #DISK_NONE
 * @memberof RevBoard
 */
_REV_EXTERN RevDiskType revGetDisk(RevBoard *board, int pos);

/**
 * Gets a disk on a board.
 * 
 * @param board RevBoard instance
 * @param x x-coordinate on a board.
 * @param y y-coordinate on a board.
 * @returns #DISK_BLACK, #DISK_WHITE or #DISK_NONE
 * @memberof RevBoard
 */
_REV_EXTERN RevDiskType revGetDiskXY(RevBoard *board, int x, int y);

/**
 * Gets mobility of the current player as a bitboard.
 * 
 * @param board RevBoard instance
 * @returns Mobility as a bitboard
 * @memberof RevBoard
 */
_REV_EXTERN RevBitboard revGetMobility(RevBoard *board);

/**
 * Gets mobility of the current player as an array of integers.
 * 
 * @note Arrays that are returned from this method should be freed with free().
 * 
 * @param board RevBoard instance
 * @returns An array of positions on a bitboard. The size of the array is equal to revGetMobilityCount().
 * @memberof RevBoard
 */
_REV_EXTERN int *revGetMobilityAsArray(RevBoard *board);

/**
 * Gets the number of legal moves for the current player.
 * 
 * @param board RevBoard instance
 * @returns Number of legal moves.
 * @memberof RevBoard
 */
_REV_EXTERN int revGetMobilityCount(RevBoard *board);

/**
 * Returns whether or not the current player has legal moves.
 * 
 * @param board RevBoard instance
 * @returns `TRUE` if the current player has legal moves, `FALSE` otherwise.
 * @memberof RevBoard
 */
_REV_EXTERN int revHasLegalMoves(RevBoard *board);

/**
 * Returns whether or not a move is legal.
 * 
 * @param board RevBoard instance
 * @param pos a position on a bitboard. pos = x + y * 8
 * @returns `TRUE` if the move is legal, `FALSE` otherwise.
 * @memberof RevBoard
 */
_REV_EXTERN int revIsLegalMove(RevBoard *board, int pos);

/**
 * Returns whether or not a move is legal.
 * 
 * @param board RevBoard instance
 * @param x x-coordinate on a board.
 * @param y y-coordinate on a board.
 * @returns `TRUE` if the move is legal, `FALSE` otherwise.
 * @memberof RevBoard
 */
_REV_EXTERN int revIsLegalMoveXY(RevBoard *board, int x, int y);

/**
 * Outputs a board to stdout.
 * 
 * @param board RevBoard instance
 * @memberof RevBoard
 */
_REV_EXTERN void revPrintBoard(RevBoard *board);

/**
 * Outputs a board with mobility to stdout.
 * 
 * @param board RevBoard instance
 * @memberof RevBoard
 */
_REV_EXTERN void revPrintBoardWithMobility(RevBoard *board);

/**
 * Calculates mobility of the current player.
 * You should call this method after editing bitboards.
 * You can get the result with revGetMobility() or revGetMobilityAsArray().
 * 
 * @note revMove(), revMoveXY(), and revChangePlayer() call this method internally.
 * 
 * @param board RevBoard instance
 * @memberof RevBoard
 */
_REV_EXTERN void revUpdateMobility(RevBoard *board);

/**
 * Changes the current player to the opposite player.
 * 
 * @note revMove() and revMoveXY() call this method internally.
 * 
 * @param board RevBoard instance
 * @memberof RevBoard
 */
_REV_EXTERN void revChangePlayer(RevBoard *board);

/**
 * Flips disks, changes the current player, and calculates mobility.
 *
 * @param board RevBoard instance
 * @param pos a position on a bitboard. pos = x + y * 8
 * @returns a bitboard that represents flipped disks.
 * @memberof RevBoard
 */
_REV_EXTERN RevBitboard revMove(RevBoard *board, int pos);

/**
 * Flips disks, changes the current player, and calculates mobility.
 * 
 * @param board RevBoard instance
 * @param x x-coordinate on a board.
 * @param y y-coordinate on a board.
 * @returns a bitboard that represents flipped disks.
 * @memberof RevBoard
 */
_REV_EXTERN RevBitboard revMoveXY(RevBoard *board, int x, int y);

/**
 * Initialize a random number generator.
 * 
 * @param seed seed of a random number generator.
 */
_REV_EXTERN void revInitGenRandom(uint32_t seed);

/**
 * Generates a random integer with the mersenne twister
 * 
 * @note This method requires revInitGenRandom() before calling.
 * 
 * @param min The lowest value to return.
 * @param max The highest value to return.
 * @returns A pseudo random value between min and max.
 */

_REV_EXTERN int revGenIntRandom(int min, int max);

/**
 * Generates a random move.
 * 
 * @note This method requires revInitGenRandom() before calling.
 * 
 * @param board RevBoard instance
 * @returns a position on a bitboard.
 * @memberof RevBoard
 */
_REV_EXTERN int revGenMoveRandom(RevBoard *board);

/**
 * Plays the game to the end randomly.
 * 
 * @note This method requires revInitGenRandom() before calling.
 * 
 * @param board RevBoard instance
 * @memberof RevBoard
 */
_REV_EXTERN void revMoveRandomToEnd(RevBoard *board);

/**
 * Calls revMoveRandomToEnd() many times for each legal move, and returns the best move that has the highest win rate.
 * 
 * @note This method requires revInitGenRandom() before calling.
 * 
 * @warning This is the Monte Carlo Serach, not MCTS.
 * 
 * @param board RevBoard instance
 * @param trials How many times this function plays the game from the current state to the end.
 * @returns a position on a bitboard.
 * @memberof RevBoard
 */
_REV_EXTERN int revGenMoveMonteCarlo(RevBoard *board, int trials);

#ifdef __cplusplus
}
#endif

#endif  // __REVERSI_INCLUDE_REVERSI_H__
