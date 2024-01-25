#include <stdio.h>
#include "reversi.h"
#include "mt.h"

const char* revGetVersion() {
    return REV_VERSION;
}

int revGetVersionAsInt() {
    return REV_VERSION_INT;
}

int revCountFirstZeros(RevBitboard b) {
#ifdef _MSC_VER
    return (int)_lzcnt_u64(b);
#else
    // __builtin_clzll(b) is undefined when b == 0
    return (int)(__builtin_clzll(b) * (b != 0) + 64 * (b == 0));
#endif
}

int revCountOnes(RevBitboard b) {
#ifdef _MSC_VER
    return (int)__popcnt64(b);
#else
    return (int)__builtin_popcountll(b);
#endif
}

int revXYToPos(int x, int y) {
    return x + y * 8;
}

int revIsTrueAt(RevBitboard b, int pos) {
    const RevBitboard mask = (RevBitboard)1 << pos;
    return (b & mask) > 0;
}

int revIsTrueAtXY(RevBitboard b, int x, int y) {
    return revIsTrueAt(b, revXYToPos(x, y));
}

static inline int *allocIntArray(int size) {
    return (int*) malloc(sizeof(int) * size);
}

static inline void revBitboardToArrayCore(RevBitboard b, int* array) {
    int *ap = array;
    for (int i = 0; i < 64; i++) {
        if (b & 1) {
            ap[0] = i;
            ap++;
        }
        b >>= 1;
    }
}

int *revBitboardToArray(RevBitboard b) {
    int *array = allocIntArray(revCountOnes(b));
    if (array == NULL) return NULL;

    revBitboardToArrayCore(b, array);
    return array;
}

RevBitboard revArrayToBitboard(int *array, int size) {
    RevBitboard b = 0;
    int *aptr = array;
    for (; aptr < array + size; aptr++) {
        b |= (RevBitboard)1 << aptr[0];
    }
    return b;
}

struct RevBoard {
    // bitboards for disks on a 8 x 8 board
    // bitboards[0] : black disks
    // bitboards[1] : white disks
    RevBitboard bitboards[2];
    RevBitboard mobility;
    RevDiskType current_player;  // 0 means black's turn. 1 means white's turn.
    int mobility_count;  // The number of legal moves.
};

RevBoard *revNewBoard() {
    RevBoard *board = (RevBoard *)malloc(sizeof(RevBoard));
    if (board != NULL)
        revInitBoard(board);
    return board;
}

void revFreeBoard(RevBoard *board) {
    free(board);
}

void revInitBoard(RevBoard *board) {
    board->current_player = DISK_BLACK;
    board->bitboards[DISK_BLACK] = 0x0000000810000000;
    board->bitboards[DISK_WHITE] = 0x0000001008000000;
    board->mobility = 0x0000102004080000;
    board->mobility_count = 4;

    // The above integers are the values when executing the following statements.
    // revSetDiskXY(board, DISK_WHITE, 3, 3);
    // revSetDiskXY(board, DISK_BLACK, 4, 3);
    // revSetDiskXY(board, DISK_BLACK, 3, 4);
    // revSetDiskXY(board, DISK_WHITE, 4, 4);
    // board->mobility = revGetMobility(board);
}

void revCopyBoard(RevBoard *src, RevBoard *trg) {
    trg->current_player = src->current_player;
    trg->bitboards[DISK_BLACK] = src->bitboards[DISK_BLACK];
    trg->bitboards[DISK_WHITE] = src->bitboards[DISK_WHITE];
    trg->mobility = src->mobility;
    trg->mobility_count = src->mobility_count;
}

RevDiskType revGetCurrentPlayer(RevBoard *board) {
    return board->current_player;
}

void revChangePlayer(RevBoard *board) {
    board->current_player = !board->current_player;
    revUpdateMobility(board);
}

RevBitboard revGetBitboard(RevBoard *board, RevDiskType disk_type) {
    return board->bitboards[disk_type];
}

void revSetBitboard(RevBoard *board, RevDiskType disk_type, RevBitboard b) {
    board->bitboards[disk_type] = b;
}

int *revGetBitboardAsArray(RevBoard *board, RevDiskType disk_type) {
    return revBitboardToArray(revGetBitboard(board, disk_type));
}

int revCountDisks(RevBoard *board, RevDiskType disk_type) {
    return revCountOnes(revGetBitboard(board, disk_type));
}

RevDiskType revGetWinner(RevBoard *board) {
    const int black_count = revCountDisks(board, DISK_BLACK);
    const int white_count = revCountDisks(board, DISK_WHITE);
    return (black_count < white_count) | ((black_count == white_count) * 2);
}

void revSetDisk(RevBoard *board, RevDiskType disk_type, int pos) {
    RevBitboard mask = (RevBitboard)1 << pos;

    // Unpopulate the specified bit
    board->bitboards[DISK_BLACK] &= ~mask;
    board->bitboards[DISK_WHITE] &= ~mask;

    // Set zeros when disk_type == DISK_NONE
    mask *= (disk_type != DISK_NONE);
    disk_type *= (disk_type != DISK_NONE);

    // Populate the specified bit
    board->bitboards[disk_type] |= mask;
}

void revSetDiskXY(RevBoard *board, RevDiskType disk_type, int x, int y) {
    revSetDisk(board, disk_type, revXYToPos(x, y));
}

RevDiskType revGetDisk(RevBoard *board, int pos) {
    uint8_t has_black = revIsTrueAt(board->bitboards[DISK_BLACK], pos);
    uint8_t has_white = revIsTrueAt(board->bitboards[DISK_WHITE], pos);
    return has_white + (!(has_black || has_white)) * DISK_NONE;
}

RevDiskType revGetDiskXY(RevBoard *board, int x, int y) {
    return revGetDisk(board, revXYToPos(x, y));
}

RevBitboard revGetMobility(RevBoard *board) {
    return board->mobility;
}

int *revGetMobilityAsArray(RevBoard *board) {
    int *array = allocIntArray(revGetMobilityCount(board));
    if (array == NULL) return NULL;

    revBitboardToArrayCore(revGetMobility(board), array);
    return array;
}

int revGetMobilityCount(RevBoard *board) {
    return board->mobility_count;
}

int revHasLegalMoves(RevBoard *board) {
    return revGetMobilityCount(board) > 0;
}

int revIsLegalMove(RevBoard *board, int pos) {
    return revIsTrueAt(revGetMobility(board), pos);
}

int revIsLegalMoveXY(RevBoard *board, int x, int y) {
    return revIsLegalMove(board, revXYToPos(x, y));
}

void revPrintBoard(RevBoard *board) {
    const char* disks = "XO.*";
    int pos = 0;
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            uint8_t char_id = revGetDisk(board, pos);
            printf(" %c", disks[(size_t)char_id]);
            pos += 1;
        }
        printf("\n");
    }
}

void revPrintBoardWithMobility(RevBoard *board) {
    const char* disks = "XO.*";
    int pos = 0;
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            uint8_t char_id = revGetDisk(board, pos);
            char_id |= revIsLegalMove(board, pos) * 3;
            printf(" %c", disks[(size_t)char_id]);
            pos += 1;
        }
        printf("\n");
    }
}

static RevBitboard getMobilityOneDirection(RevBitboard p_board, RevBitboard masked_o, int shift) {
    RevBitboard mobility;
    RevBitboard flip, pre;
    const int shift_double = shift * 2;

    flip = masked_o & (p_board << shift);
    flip |= masked_o & (flip << shift);
    pre = masked_o & (masked_o << shift);
    flip |= pre & (flip << shift_double);
    flip |= pre & (flip << shift_double);
    mobility = flip << shift;
    flip = masked_o & (p_board >> shift);
    flip |= masked_o & (flip >> shift);
    pre = masked_o & (masked_o >> shift);
    flip |= pre & (flip >> shift_double);
    flip |= pre & (flip >> shift_double);
    mobility |= flip >> shift;
    return mobility;
}

void revUpdateMobility(RevBoard *board) {
    const RevDiskType p_disk_type = board->current_player;
    const RevDiskType o_disk_type = !p_disk_type;
    const RevBitboard p_board = board->bitboards[p_disk_type];
    const RevBitboard o_board = board->bitboards[o_disk_type];

    RevBitboard mobility;
    const RevBitboard masked_o = o_board & 0x7e7e7e7e7e7e7e7e;

    mobility = getMobilityOneDirection(p_board, masked_o, 1);  // horizontal
    mobility |= getMobilityOneDirection(p_board, o_board, 8);  // vertical
    mobility |= getMobilityOneDirection(p_board, masked_o, 7);  // diagonal
    mobility |= getMobilityOneDirection(p_board, masked_o, 9);
    mobility &= ~(p_board | o_board);
    board->mobility = mobility;
    board->mobility_count = revCountOnes(mobility);
}

static RevBitboard flipDisksOneDirection(int pos,
                                         RevBitboard p_board, RevBitboard o_board,
                                         RevBitboard masked_o,
                                         RevBitboard mask_r, RevBitboard mask_l) {
    RevBitboard outflank, flipped;
    RevBitboard mask = mask_r >> (63 - pos);
    outflank = (0x8000000000000000 >> revCountFirstZeros(~masked_o & mask)) & p_board;
    flipped  = (-outflank * 2) & mask;

    mask = mask_l << pos;
    outflank = mask & ((masked_o | ~mask) + 1) & p_board;
    flipped |= (outflank - (RevBitboard)(outflank != 0)) & mask;
    return flipped;
}

static RevBitboard flipDisks(RevBoard *board, int pos) {
    RevDiskType p_disk_type = board->current_player;
    RevDiskType o_disk_type = !p_disk_type;
    RevBitboard p_board = board->bitboards[p_disk_type];
    RevBitboard o_board = board->bitboards[o_disk_type];

    RevBitboard flipped;
    RevBitboard masked_o = o_board & 0x7e7e7e7e7e7e7e7e;
    flipped = flipDisksOneDirection(pos, p_board, o_board, o_board,
                                    0x0080808080808080, 0x0101010101010100);  // horizontal
    flipped |= flipDisksOneDirection(pos, p_board, o_board, masked_o,
                                     0x7f00000000000000, 0x00000000000000fe);  // vertical
    flipped |= flipDisksOneDirection(pos, p_board, o_board, masked_o,
                                     0x0102040810204000, 0x0002040810204080);  // diagonal
    flipped |= flipDisksOneDirection(pos, p_board, o_board, masked_o,
                                     0x0040201008040201, 0x8040201008040200);

    board->bitboards[p_disk_type] = p_board ^ flipped;
    board->bitboards[o_disk_type] = o_board ^ flipped;
    return flipped;
}

RevBitboard revMove(RevBoard *board, int pos) {
    RevBitboard flipped = flipDisks(board, pos);
    revSetDisk(board, board->current_player, pos);
    revChangePlayer(board);
    return flipped;
}

RevBitboard revMoveXY(RevBoard *board, int x, int y) {
    return revMove(board, revXYToPos(x, y));
}

void revInitGenRandom(uint32_t seed) {
    init_genrand(seed);
}

int revGenIntRandom(int min, int max) {
    return genrand_int32() % (max - min + 1) + min;
}

int revGenMoveRandom(RevBoard *board) {
    const int mobility_count = revGetMobilityCount(board);
    RevBitboard m = revGetMobility(board);
    int n = revGenIntRandom(0, mobility_count - 1);
    int i = -1;
    // Get the position of the Nth poplated bit.
    do {
        n -= (m & 1);  // Substract 1 if found a populated bit.
        m >>= 1;
        i++;
    } while (n >= 0);
    return i;
}

void revMoveRandomToEnd(RevBoard *board) {
    // Call revGenMoveRandom() until no one can put disks.
    while (revHasLegalMoves(board)) {
        int move = revGenMoveRandom(board);
        revMove(board, move);
        if (!revHasLegalMoves(board)) {
            revChangePlayer(board);
        }
    }
}

int revGenMoveMonteCarlo(RevBoard *board, int trials) {
    const int mobility_count = revGetMobilityCount(board);
    int *ma = revGetMobilityAsArray(board);
    const RevDiskType p_disk_type = revGetCurrentPlayer(board);
    const RevDiskType o_disk_type = !p_disk_type;

    RevBoard *tmp_board = revNewBoard();
    RevBoard *tmp_board2 = revNewBoard();
    int max_win = 0;
    int best_move = ma[0];
    trials /= mobility_count;
    int *mptr = ma;
    for (; mptr < ma + mobility_count; mptr++) {
        int m = mptr[0];
        revCopyBoard(board, tmp_board);
        revMove(tmp_board, m);
        int win = 0;
        for (int i = 0; i < trials; i++) {
            revCopyBoard(tmp_board, tmp_board2);
            revMoveRandomToEnd(tmp_board2);
            win += revCountDisks(tmp_board2, p_disk_type) > revCountDisks(tmp_board2, o_disk_type);
        }
        if (max_win < win) {
            max_win = win;
            best_move = m;
        }
    }
    revFreeBoard(tmp_board);
    revFreeBoard(tmp_board2);
    free(ma);
    return best_move;
}
