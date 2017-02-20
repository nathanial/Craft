//
// Created by nathan on 2/19/17.
//

#include "TerrainFeature.h"
#include "../chunk.h"


void TerrainFeature::add_to_chunk(Chunk &chunk, int x, int h, int z){
    for(auto &kv : this->create()){
        int ox , y, oz;
        std::tie(ox,y,oz) = std::get<0>(kv);
        chunk.set_block(x + ox, h + y, z + oz, std::get<1>(kv));
    }
}