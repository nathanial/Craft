//
// Created by nathan on 1/29/17.
//

extern "C" {
    #include <noise.h>
}
#include "mountains.h"
#include "../item.h"

void Mountains::create_chunk(ChunkPtr chunk, int p, int q) {
    for (int dx = 0; dx < CHUNK_SIZE; dx++) {
        for (int dz = 0; dz < CHUNK_SIZE; dz++) {
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            float f = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2);
            float g = simplex2(-x * 0.01, -z * 0.01, 2, 0.9, 2);
            float mountain_factor = (x / 32.0f) / 2.0;
            if(mountain_factor > 3) {
                mountain_factor = 3;
            }
            if(mountain_factor < 1){
                mountain_factor = 1;
            }
            int mh = g * (32 * mountain_factor) + 16;
            int h = f * mh;
            int w = 1;
            int t = 12;
            if (h <= t) {
                h = t;
                w = 2;
            }
            // sand and grass terrain
            for (int y = 0; y < h; y++) {
                chunk->set_block(x, y, z, w);
            }

            if(simplex2(x, z, 6, 0.5, 2) > 0.80){
                chunk->set_block(x,h+10,z, BEACON);
            }
        }
    }
}