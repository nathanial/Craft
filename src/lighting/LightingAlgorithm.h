//
// Created by Nathanial Hartman on 3/17/17.
//

#ifndef CRAFT_LIGHTINGALGORITHM_H
#define CRAFT_LIGHTINGALGORITHM_H

#include "../block_map.h"
#include "../chunk/chunk.h"

class LightingAlgorithm {
    virtual std::unique_ptr<BigBlockMap> light(int p, int q, const BigBlockMap &opaque, const ChunkNeighbors& neighbors) = 0;
};

#endif //CRAFT_LIGHTINGALGORITHM_H
