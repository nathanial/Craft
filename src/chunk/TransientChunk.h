//
// Created by Nathanial Hartman on 3/4/17.
//

#ifndef CRAFT_TRANSIENTCHUNK_H
#define CRAFT_TRANSIENTCHUNK_H

#include <functional>
#include "../config.h"
#include "../block_map.h"

class Chunk;

class TransientChunk {
public:
    const int p, q;
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;

    TransientChunk(int p, int q);
    int set_block(int x, int y, int z, char w);
    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    void foreach_block(std::function<void(int, int, int, char)> func) const;

    std::shared_ptr<Chunk> immutable() const;
};


#endif //CRAFT_TRANSIENTCHUNK_H
