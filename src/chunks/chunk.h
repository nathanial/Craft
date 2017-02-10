//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/glew.h>
#include "../block_map.h"
#include "../height_map.h"
#include "../sign.h"
#include <memory>
#include <mutex>

class Attrib;
class Chunk;

typedef std::shared_ptr<Chunk> ChunkPtr;
typedef std::tuple<int, int> ChunkPosition;

struct ChunkPositionHash : public std::unary_function<ChunkPosition, std::size_t>
{
    std::size_t operator()(const ChunkPosition& k) const
    {
        return std::get<0>(k) ^ (std::get<1>(k) << 1);
    }
};
typedef std::unordered_map<ChunkPosition, ChunkPtr, ChunkPositionHash> Neighborhood;


class Chunk {
private:
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;
    int _p, _q; // chunk position
public:
    Chunk(int p, int q);
    ~Chunk();

    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    int set_block(int x, int y, int z, char w);
    int get_block_raw(int x, int y, int z) const;

    int distance(int p, int q);

    void foreach_block(std::function<void (int, int, int, char)> func) const;

    int p() const;
    int q() const;
};


int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
