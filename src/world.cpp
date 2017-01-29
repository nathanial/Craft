#include "config.h"
extern "C" {
    #include "noise.h"
}
#include "world.h"
#include "item.h"
#include "biomes/rolling_planes.h"

void create_world(ChunkPtr chunk, int p, int q) {
    if(!chunk){
        throw new std::invalid_argument("Blocks must not be null");
    }

    RollingPlanes planes;
    planes.create_chunk(chunk, p, q);
}
