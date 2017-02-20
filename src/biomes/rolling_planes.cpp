//
// Created by nathan on 1/29/17.
//

extern "C" {
#include "noise.h"
}
#include "rolling_planes.h"
#include "../item.h"
#include "../terrain_features/trees/OakTree.h"

void RollingPlanes::create_chunk(ChunkPtr chunk, int p, int q) {
    for (int dx = 0; dx < CHUNK_SIZE; dx++) {
        for (int dz = 0; dz < CHUNK_SIZE; dz++) {
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            float f = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2);
            float g = simplex2(-x * 0.01, -z * 0.01, 2, 0.9, 2);
            int mh = g * 32 + 16;
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
                chunk->set_block(x,h+1,z, BEACON);
            }
            if (w == 1) {
                if (SHOW_PLANTS) {
                    // grass
                    if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                        chunk->set_block(x, h, z, 17);
                    }
                    // flowers
                    if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                        int w = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                        chunk->set_block(x, h, z, w);
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
                    tree.add_to_chunk(*chunk, x, h, z);
                }
            }

            // clouds
            if (SHOW_CLOUDS) {
                for (int y = 64; y < 72; y++) {
                    if (simplex3(
                            x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75)
                    {
                        chunk->set_block(x, y, z, 16);
                    }
                }
            }
        }
    }
}