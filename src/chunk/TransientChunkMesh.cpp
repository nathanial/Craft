//
// Created by Nathanial Hartman on 3/5/17.
//

#include "TransientChunkMesh.h"
#include "ChunkMesh.h"
#include "../util.h"


std::shared_ptr<ChunkMesh> TransientChunkMesh::immutable() const {
    return std::make_shared<ChunkMesh>(
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
    std::vector<GLfloat> emptyVertices;
    this->buffer = gen_buffer(vertices);
    this->vertices = emptyVertices;
}