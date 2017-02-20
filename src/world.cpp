#include "config.h"
extern "C" {
    #include "noise.h"
}
#include "world.h"
#include "item.h"
#include "biomes/rolling_planes.h"
#include "biomes/mountains.h"
#include "biomes/flatland.h"

void create_world(ChunkPtr chunk, int p, int q) {
    if(!chunk){
        throw new std::invalid_argument("Blocks must not be null");
    }

    Mountains mountains;
    mountains.create_chunk(chunk, p, q);

}

