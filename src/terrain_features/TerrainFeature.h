//
// Created by nathan on 2/19/17.
//

#ifndef CRAFT_TERRAINFEATURE_H
#define CRAFT_TERRAINFEATURE_H


#include <unordered_map>
#include <tuple>
#include "../chunk/TransientChunk.h"

typedef std::tuple<int, int, int> BlockPosition;

class Chunk;

struct BlockPositionHash : public std::unary_function<BlockPosition, std::size_t>  {
    std::size_t operator()(const BlockPosition& k) const {
        return std::get<0>(k) ^ (std::get<1>(k) << 1) ^ (std::get<2>(k) << 2);
    }
};

typedef std::unordered_map<BlockPosition, int, BlockPositionHash> Blocks;

class TerrainFeature {
public:
    virtual Blocks create() = 0;

    void add_to_chunk(TransientChunk &chunk, int x, int y, int z);
};


#endif //CRAFT_TERRAINFEATURE_H
