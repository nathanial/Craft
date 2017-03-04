//
// Created by nathan on 1/29/17.
//

#ifndef CRAFT_MOUNTAINS_H
#define CRAFT_MOUNTAINS_H

#include "biome.h"

class Mountains : public Biome {
public:
    void create_chunk(TransientChunk& chunk, int p, int q) override;
};

#endif //CRAFT_MOUNTAINS_H
