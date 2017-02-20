//
// Created by nathan on 2/5/17.
//

#include "flatland.h"
#include "../item.h"

void Flatland::create_chunk(Chunk& chunk, int p, int q) {
    for (int dx = 0; dx < CHUNK_SIZE; dx++) {
        for (int dz = 0; dz < CHUNK_SIZE; dz++) {
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            for(int y = 0; y < 10; y++){
                chunk.set_block(x,y,z,GRASS);
            }
        }
    }
}