//
// Created by Nathanial Hartman on 3/2/17.
//

#include <iostream>
#include "ChunkMesh.h"
#include "../draw.h"
#include "../util.h"
#include "./TransientChunkMesh.h"

int ChunkMesh::draw(Attrib *attrib) const {
    if(!buffer){
        std::cout << "Can't draw without a buffer" << std::endl;
        throw "Can't draw without a buffer";
    }
    draw_triangles_3d_ao(attrib, buffer, faces * 6);
    return faces;
}

std::shared_ptr<TransientChunkMesh> ChunkMesh::transient() const {
    auto transient = std::make_shared<TransientChunkMesh>();
    transient->buffer = this->buffer;
    transient->faces = this->faces;
    transient->maxy = this->maxy;
    transient->miny  = this->miny;
    transient->vertices = this->vertices;
    return transient;
}