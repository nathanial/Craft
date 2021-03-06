//
// Created by nathan on 1/29/17.
//

extern "C" {
    #include <noise.h>
}
#include "mountains.h"
#include "../item.h"
#include "../terrain_features/trees/OakTree.h"

void Mountains::create_chunk(TransientChunk& chunk, int p, int q) {
    for (int dx = 0; dx < CHUNK_SIZE; dx++) {
        for (int dz = 0; dz < CHUNK_SIZE; dz++) {
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;

            float mountain_factor;
            if(x < 0){
                mountain_factor = (ABS(x + CHUNK_SIZE * 3) / 32.0f) / 2.0;
            } else {
                mountain_factor = (ABS(x) / 32.0f) / 2.0;
            }

            if(mountain_factor > 3.5) {
                mountain_factor = 3.5;
            }
            if(mountain_factor < 1){
                mountain_factor = 1;
            }

            float stretch_factor = 0.005;
            float f = simplex2(x * stretch_factor, z * stretch_factor, 4, 0.5, 2);
            float g = simplex2(-x * stretch_factor, -z * stretch_factor, 2, 0.9, 2);

            int mh = g * (40 * mountain_factor) + 16;
            int h = f * mh;
            int w = GRASS;
            int t = 12;
            if (h <= t) {
                h = t;
                w = SAND;
            }
            // sand and grass terrain
            for (int y = 0; y < h; y++) {
                if(w == GRASS && y < (h - 1)){
                    chunk.set_block(x, y, z, DIRT);
                } else {
                    chunk.set_block(x, y, z, w);
                }
            }

            if(simplex2(x, z, 6, 0.5, 2) > 0.78){
                chunk.set_block(x,h+1,z, BEACON);
            }

            if (SHOW_PLANTS) {
                // grass
                if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                    chunk.set_block(x, h, z, 17);
                }
                // flowers
                if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                    int w = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                    chunk.set_block(x, h, z, w);
                }
            }
            // trees
            int ok = SHOW_TREES;
            if (dx - 4 < 0 || dz - 4 < 0 ||
                dx + 4 >= CHUNK_SIZE || dz + 4 >= CHUNK_SIZE)
            {
                ok = 0;
            }
            if (ok && simplex2(x, z, 6, 0.5, 2) > 0.84) {
                OakTree tree;
                tree.add_to_chunk(chunk, x,h,z);
            }
        }
    }
}