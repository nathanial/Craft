//
// Created by nathan on 1/29/17.
//

#ifndef CRAFT_BIOME_H
#define CRAFT_BIOME_H

#include "../chunk.h"

class Biome {
public:
    virtual void create_chunk(Chunk& chunk, int p, int q) = 0;
};

#endif //CRAFT_BIOME_H
