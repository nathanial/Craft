//
// Created by nathan on 1/29/17.
//

extern "C" {
#include <noise.h>
}
#include "flatland.h"
#include "../item.h"

void Flatland::create_chunk(ChunkPtr chunk, int p, int q) {
    for (int dx = 0; dx < CHUNK_SIZE; dx++) {
        for (int dz = 0; dz < CHUNK_SIZE; dz++) {
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            chunk->set_block(x,0,z, GRASS);
            if(dx == 0) {
                chunk->set_block(x, 0, z, STONE);
            }
            else if(dx == CHUNK_SIZE - 1){
                chunk->set_block(x, 0, z, DIRT);
            }
            else if(dz == 0){
                chunk->set_block(x, 0, z, PLANK);
            } else if(dz == CHUNK_SIZE - 1){
                chunk->set_block(x, 0, z, SNOW);
            }
        }
    }
}