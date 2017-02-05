//
// Created by nathan on 2/5/17.
//

#ifndef CRAFT_FLATLAND_H
#define CRAFT_FLATLAND_H


#include "biome.h"

class Flatland : public Biome {
public:
    void create_chunk(ChunkPtr chunk, int p, int q) override;
};


#endif //CRAFT_FLATLAND_H
