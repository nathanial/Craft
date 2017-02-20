//
// Created by nathan on 2/19/17.
//

#ifndef CRAFT_TERRAINFEATURE_H
#define CRAFT_TERRAINFEATURE_H


#include <unordered_map>
#include <tuple>

typedef std::tuple<int, int, int> BlockPosition;

struct BlockPositionHash : public std::unary_function<BlockPosition, std::size_t>  {
    std::size_t operator()(const BlockPosition& k) const {
        return std::get<0>(k) ^ (std::get<1>(k) << 1) ^ (std::get<2>(k) << 2);
    }
};

typedef std::unordered_map<BlockPosition, int, BlockPositionHash> Blocks;

class TerrainFeature {
public:
    virtual Blocks create() = 0;
};


#endif //CRAFT_TERRAINFEATURE_H
