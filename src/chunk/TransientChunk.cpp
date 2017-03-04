//
// Created by Nathanial Hartman on 3/4/17.
//

#include "TransientChunk.h"

TransientChunk::TransientChunk(int p, int q) :
    p(p), q(q),
    blocks(std::make_unique<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>>())
{
}
int TransientChunk::set_block(int x, int y, int z, char w) {
    return this->blocks->set(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE, w);
}
int TransientChunk::get_block(int x, int y, int z) const {
    return this->blocks->get(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE);
}
int TransientChunk::get_block_or_zero(int x, int y, int z) const {
    return this->blocks->get_or_default(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE, 0);
}
void TransientChunk::foreach_block(std::function<void(int, int, int, char)> func) const {
    this->blocks->each([&](int x, int y, int z, char w){
        func(x + this->p * CHUNK_SIZE, y, z + this->q * CHUNK_SIZE, w);
    });
}