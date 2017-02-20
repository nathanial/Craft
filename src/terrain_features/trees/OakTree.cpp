//
// Created by nathan on 2/19/17.
//

#include <noise.h>
#include "OakTree.h"

Blocks OakTree::create() {
    Blocks blocks;
    for (int y = 3; y < 8; y++) {
        for (int ox = -3; ox <= 3; ox++) {
            for (int oz = -3; oz <= 3; oz++) {
                int d = (ox * ox) + (oz * oz) +
                        (y - 4) * (y - 4);
                if (d < 11) {
                    blocks[std::make_tuple(ox, y, oz)] = 15;
                }
            }
        }
    }
    for (int y = 0; y < 7; y++) {
        blocks[std::make_tuple(0,y,0)] = 5;
    }
    return blocks;
}