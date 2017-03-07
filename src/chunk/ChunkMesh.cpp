//
// Created by Nathanial Hartman on 3/2/17.
//

#include <iostream>
#include "ChunkMesh.h"
#include "../draw.h"
#include "../util.h"
#include "./TransientChunkMesh.h"

int ChunkMesh::draw(Attrib *attrib) const {
    if(buffer){
        draw_triangles_3d_ao(attrib, buffer, faces * 6);
        return faces;
    } else {
        std::cout << "No Buffer" << std::endl;
        return 0;
    }
}

bool ChunkMesh::is_ready_to_draw() const {
    return buffer && dirty;
}



std::shared_ptr<TransientChunkMesh> ChunkMesh::transient() const {
    auto transient = std::make_shared<TransientChunkMesh>();
    transient->dirty = this->dirty;
    transient->buffer = this->buffer;
    transient->faces = this->faces;
    transient->maxy = this->maxy;
    transient->miny  = this->miny;
    transient->vertices = this->vertices;
    return transient;
}