//
// Created by Nathanial Hartman on 3/17/17.
//

#ifndef CRAFT_SCANLINEFILL_H
#define CRAFT_SCANLINEFILL_H


#include "../block_map.h"
#include "../chunk/chunk.h"
#include "LightingAlgorithm.h"

class ScanlineFill : public LightingAlgorithm {
public:
    std::unique_ptr<BigBlockMap> light(int p, int q, const BigBlockMap &opaque, const ChunkNeighbors& neighbors) override;
};


#endif //CRAFT_SCANLINEFILL_H
