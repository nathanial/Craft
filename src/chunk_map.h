//
// Created by nathan on 2/8/17.
//

#ifndef CRAFT_CHUNKMAP_H
#define CRAFT_CHUNKMAP_H


#include <tuple>
#include <mutex>
#include <shared_mutex>
#include "chunk.h"

typedef std::tuple<int, int> ChunkPosition;

struct ChunkPositionHash : public std::unary_function<ChunkPosition, std::size_t>
{
    std::size_t operator()(const ChunkPosition& k) const
    {
        return std::get<0>(k) ^ (std::get<1>(k) << 1);
    }
};

class ChunkMap {
private:
    std::unordered_map<ChunkPosition, ChunkPtr, ChunkPositionHash> chunks;
    mutable std::shared_mutex mtx;
public:

    ChunkPtr get_chunk(const ChunkPosition &position) const;
    void set_chunk(const ChunkPosition &position, ChunkPtr chunk);
    void clear();
    void each_chunk(std::function<void (ChunkPtr chunk)> func);
    void remove(const ChunkPosition &position);
    unsigned long size() const;
};


#endif //CRAFT_CHUNKMAP_H
