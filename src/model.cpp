#include "model.h"
#include "chunk.h"
#include <stdio.h>
#include <string.h>

extern Model *g;

ChunkPtr Model::get_chunk(int i) {
    return this->chunks + i;
}

void Model::clear_chunks(){
    memset(g->chunks, 0, sizeof(Chunk) * MAX_CHUNKS);
}aaaaaaaaaaa