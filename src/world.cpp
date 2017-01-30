#include "config.h"
extern "C" {
    #include "noise.h"
}
#include "world.h"
#include "item.h"
#include "biomes/rolling_planes.h"
#include "biomes/mountains.h"

void create_world(ChunkPtr chunk, int p, int q) {
    if(!chunk){
        throw new std::invalid_argument("Blocks must not be null");
    }

    RollingPlanes planes;
    Mountains mountains;
    if(p > 0) {
        mountains.create_chunk(chunk, p, q);
    } else {
        planes.create_chunk(chunk, p, q);
    }
}

