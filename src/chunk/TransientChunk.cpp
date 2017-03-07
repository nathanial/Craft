//
// Created by Nathanial Hartman on 3/4/17.
//

#include "TransientChunk.h"
#include "Chunk.h"


static int BLOCK_INDEX(int x, int y, int z){
    return x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z;
}

TransientChunk::TransientChunk(int p, int q) :
    p(p), q(q),
    blocks(std::vector<char>())
{
    blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
    std::fill(blocks.begin(), blocks.end(), 0);
}
int TransientChunk::set_block(int x, int y, int z, char w) {
    return this->blocks[BLOCK_INDEX(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE)] = w;
}
int TransientChunk::get_block(int x, int y, int z) const {
    return this->blocks[BLOCK_INDEX(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE)];
}
int TransientChunk::get_block_or_zero(int x, int y, int z) const {
    int index = BLOCK_INDEX(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE);
    if(index >= this->blocks.size()){
        return 0;
    }
    return this->blocks[index];
}
void TransientChunk::foreach_block(std::function<void(int, int, int, char)> func) const {
    for(int x = 0; x < CHUNK_SIZE; x++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            for(int z = 0; z < CHUNK_SIZE; z++){
                char w = this->blocks[BLOCK_INDEX(x,y,z)];
                func(x + this->p * CHUNK_SIZE, y, z + this->q * CHUNK_SIZE, w);
            }
        }
    }
}

std::shared_ptr<Chunk> TransientChunk::immutable() const {
    return std::make_shared<Chunk>(p, q, blocks);
}