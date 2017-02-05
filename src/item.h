#ifndef _item_h_
#define _item_h_

#include "util.h"

#define EMPTY 0
#define GRASS 1
#define SAND 2
#define STONE 3
#define BEACON 4
#define WOOD 5
#define CEMENT 6
#define DIRT 7
#define PLANK 8
#define SNOW 9
#define GLASS 10
#define COBBLE 11
#define LIGHT_STONE 12
#define DARK_STONE 13
#define CHEST 14
#define LEAVES 15
#define CLOUD 16
#define TALL_GRASS 17
#define YELLOW_FLOWER 18
#define RED_FLOWER 19
#define PURPLE_FLOWER 20
#define SUN_FLOWER 21
#define WHITE_FLOWER 22
#define BLUE_FLOWER 23
#define COLOR_00 32
#define COLOR_01 33
#define COLOR_02 34
#define COLOR_03 35
#define COLOR_04 36
#define COLOR_05 37
#define COLOR_06 38
#define COLOR_07 39
#define COLOR_08 40
#define COLOR_09 41
#define COLOR_10 42
#define COLOR_11 43
#define COLOR_12 44
#define COLOR_13 45
#define COLOR_14 46
#define COLOR_15 47
#define COLOR_16 48
#define COLOR_17 49
#define COLOR_18 50
#define COLOR_19 51
#define COLOR_20 52
#define COLOR_21 53
#define COLOR_22 54
#define COLOR_23 55
#define COLOR_24 56
#define COLOR_25 57
#define COLOR_26 58
#define COLOR_27 59
#define COLOR_28 60
#define COLOR_29 61
#define COLOR_30 62
#define COLOR_31 63


const int items[] = {
        // items the user can build
        GRASS,
        SAND,
        STONE,
        BEACON,
        WOOD,
        CEMENT,
        DIRT,
        PLANK,
        SNOW,
        GLASS,
        COBBLE,
        LIGHT_STONE,
        DARK_STONE,
        CHEST,
        LEAVES,
        TALL_GRASS,
        YELLOW_FLOWER,
        RED_FLOWER,
        PURPLE_FLOWER,
        SUN_FLOWER,
        WHITE_FLOWER,
        BLUE_FLOWER,
        COLOR_00,
        COLOR_01,
        COLOR_02,
        COLOR_03,
        COLOR_04,
        COLOR_05,
        COLOR_06,
        COLOR_07,
        COLOR_08,
        COLOR_09,
        COLOR_10,
        COLOR_11,
        COLOR_12,
        COLOR_13,
        COLOR_14,
        COLOR_15,
        COLOR_16,
        COLOR_17,
        COLOR_18,
        COLOR_19,
        COLOR_20,
        COLOR_21,
        COLOR_22,
        COLOR_23,
        COLOR_24,
        COLOR_25,
        COLOR_26,
        COLOR_27,
        COLOR_28,
        COLOR_29,
        COLOR_30,
        COLOR_31
};

const int item_count = sizeof(items) / sizeof(int);

const int blocks[256][6] = {
        // w => (left, right, top, bottom, front, back) tiles
        {0, 0, 0, 0, 0, 0}, // 0 - empty
        {16, 16, 32, 0, 16, 16}, // 1 - grass
        {1, 1, 1, 1, 1, 1}, // 2 - sand
        {2, 2, 2, 2, 2, 2}, // 3 - stone
        {3, 3, 3, 3, 3, 3}, // 4 - brick
        {20, 20, 36, 4, 20, 20}, // 5 - wood
        {5, 5, 5, 5, 5, 5}, // 6 - cement
        {6, 6, 6, 6, 6, 6}, // 7 - dirt
        {7, 7, 7, 7, 7, 7}, // 8 - plank
        {24, 24, 40, 8, 24, 24}, // 9 - snow
        {9, 9, 9, 9, 9, 9}, // 10 - glass
        {10, 10, 10, 10, 10, 10}, // 11 - cobble
        {11, 11, 11, 11, 11, 11}, // 12 - light stone
        {12, 12, 12, 12, 12, 12}, // 13 - dark stone
        {13, 13, 13, 13, 13, 13}, // 14 - chest
        {14, 14, 14, 14, 14, 14}, // 15 - leaves
        {15, 15, 15, 15, 15, 15}, // 16 - cloud
        {0, 0, 0, 0, 0, 0}, // 17
        {0, 0, 0, 0, 0, 0}, // 18
        {0, 0, 0, 0, 0, 0}, // 19
        {0, 0, 0, 0, 0, 0}, // 20
        {0, 0, 0, 0, 0, 0}, // 21
        {0, 0, 0, 0, 0, 0}, // 22
        {0, 0, 0, 0, 0, 0}, // 23
        {0, 0, 0, 0, 0, 0}, // 24
        {0, 0, 0, 0, 0, 0}, // 25
        {0, 0, 0, 0, 0, 0}, // 26
        {0, 0, 0, 0, 0, 0}, // 27
        {0, 0, 0, 0, 0, 0}, // 28
        {0, 0, 0, 0, 0, 0}, // 29
        {0, 0, 0, 0, 0, 0}, // 30
        {0, 0, 0, 0, 0, 0}, // 31
        {176, 176, 176, 176, 176, 176}, // 32
        {177, 177, 177, 177, 177, 177}, // 33
        {178, 178, 178, 178, 178, 178}, // 34
        {179, 179, 179, 179, 179, 179}, // 35
        {180, 180, 180, 180, 180, 180}, // 36
        {181, 181, 181, 181, 181, 181}, // 37
        {182, 182, 182, 182, 182, 182}, // 38
        {183, 183, 183, 183, 183, 183}, // 39
        {184, 184, 184, 184, 184, 184}, // 40
        {185, 185, 185, 185, 185, 185}, // 41
        {186, 186, 186, 186, 186, 186}, // 42
        {187, 187, 187, 187, 187, 187}, // 43
        {188, 188, 188, 188, 188, 188}, // 44
        {189, 189, 189, 189, 189, 189}, // 45
        {190, 190, 190, 190, 190, 190}, // 46
        {191, 191, 191, 191, 191, 191}, // 47
        {192, 192, 192, 192, 192, 192}, // 48
        {193, 193, 193, 193, 193, 193}, // 49
        {194, 194, 194, 194, 194, 194}, // 50
        {195, 195, 195, 195, 195, 195}, // 51
        {196, 196, 196, 196, 196, 196}, // 52
        {197, 197, 197, 197, 197, 197}, // 53
        {198, 198, 198, 198, 198, 198}, // 54
        {199, 199, 199, 199, 199, 199}, // 55
        {200, 200, 200, 200, 200, 200}, // 56
        {201, 201, 201, 201, 201, 201}, // 57
        {202, 202, 202, 202, 202, 202}, // 58
        {203, 203, 203, 203, 203, 203}, // 59
        {204, 204, 204, 204, 204, 204}, // 60
        {205, 205, 205, 205, 205, 205}, // 61
        {206, 206, 206, 206, 206, 206}, // 62
        {207, 207, 207, 207, 207, 207}, // 63
};

const int plants[256] = {
        // w => tile
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 16
        48, // 17 - tall grass
        49, // 18 - yellow flower
        50, // 19 - red flower
        51, // 20 - purple flower
        52, // 21 - sun flower
        53, // 22 - white flower
        54, // 23 - blue flower
};

inline int is_plant(int w) {
    switch (w) {
        case TALL_GRASS:
        case YELLOW_FLOWER:
        case RED_FLOWER:
        case PURPLE_FLOWER:
        case SUN_FLOWER:
        case WHITE_FLOWER:
        case BLUE_FLOWER:
            return 1;
        default:
            return 0;
    }
}

inline bool is_obstacle(int w) {
    w = ABS(w);
    if (is_plant(w)) {
        return false;
    }
    switch (w) {
        case EMPTY:
        case CLOUD:
            return false;
        default:
            return true;
    }
}

inline int is_destructable(int w) {
    switch (w) {
        case EMPTY:
        case CLOUD:
            return 0;
        default:
            return 1;
    }
}

inline bool is_light(int w){
    switch(w){
        case BEACON:
            return true;
        default:
            return false;
    }
}

inline int is_transparent(int w) {
    if (w == EMPTY) {
        return 1;
    }
    w = ABS(w);
    if (is_plant(w)) {
        return 1;
    }
    switch (w) {
        case EMPTY:
        case GLASS:
        case LEAVES:
            return 1;
        default:
            return 0;
    }
}

#endif
