//
// Created by nathan on 1/29/17.
//

#ifndef CRAFT_ROLLING_PLANES_H_H
#define CRAFT_ROLLING_PLANES_H_H

#include "biome.h"

class RollingPlanes : public Biome {
public:
    void create_chunk(TransientChunk& chunk, int p, int q) override;
};

#endif //CRAFT_ROLLING_PLANES_H_H
