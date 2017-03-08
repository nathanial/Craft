//
// Created by Nathanial Hartman on 3/5/17.
//

#include "TransientChunkMesh.h"
#include "ChunkMesh.h"
#include "../util.h"


ChunkMesh TransientChunkMesh::immutable() const {
    return ChunkMesh(
        miny, maxy, faces, dirty, buffer, vertices
    );
}

void TransientChunkMesh::generate_buffer() {
    if(buffer) {
        del_buffer(buffer);
        if(vertices.size() == 0){
            this->buffer = 0;
        }
    }
    this->buffer = gen_buffer(vertices);
}