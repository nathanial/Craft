#include "config.h"
extern "C" {
    #include "noise.h"
}
#include "world.h"
#include "map.h"
#include "item.h"

void create_world(ChunkPtr chunk, int p, int q) {
    if(!chunk){
        throw new std::invalid_argument("Blocks must not be null");
    }
    for (int dx = 0; dx < CHUNK_SIZE; dx++) {
        for (int dz = 0; dz < CHUNK_SIZE; dz++) {
            int flag = 1;
            if (dx < 0 || dz < 0 || dx >= CHUNK_SIZE || dz >= CHUNK_SIZE) {
                flag = -1;
            }
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
                chunk->set_block(x, y, z, w * flag);
            }

            if (w == 1) {
                if (SHOW_PLANTS) {
                    // grass
                    if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                        chunk->set_block(x, h, z, 17 * flag);
                    }
                    // flowers
                    if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                        int w = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                        chunk->set_block(x, h, z, w * flag);
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
                    for (int y = h + 3; y < h + 8; y++) {
                        for (int ox = -3; ox <= 3; ox++) {
                            for (int oz = -3; oz <= 3; oz++) {
                                int d = (ox * ox) + (oz * oz) +
                                    (y - (h + 4)) * (y - (h + 4));
                                if (d < 11) {
                                    chunk->set_block(x + ox, y, z + oz, 15);
                                }
                            }
                        }
                    }
                    for (int y = h; y < h + 7; y++) {
                        chunk->set_block(x, y, z, 5);
                    }
                }

            } else {
                if(simplex2(x,z, 12, 0.2, 2) > 0.90){
                    chunk->set_block(x,h+5,z,BEACON);
                }
            }
            // clouds
            if (SHOW_CLOUDS) {
                for (int y = 64; y < 72; y++) {
                    if (simplex3(
                        x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75)
                    {
                        chunk->set_block(x, y, z, 16 * flag);
                    }
                }
            }
        }
    }
}
