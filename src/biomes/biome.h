//
// Created by nathan on 1/29/17.
//

#ifndef CRAFT_BIOME_H
#define CRAFT_BIOME_H

#include "../chunk/chunk.h"
#include "../chunk/TransientChunk.h"

class Biome {
public:
    virtual void create_chunk(TransientChunk& chunk, int p, int q) = 0;
};

#endif //CRAFT_BIOME_H
