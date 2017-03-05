//
// Created by Nathanial Hartman on 3/2/17.
//

#include "ChunkMesh.h"
#include "../draw.h"
#include "../util.h"
#include "./TransientChunkMesh.h"

int ChunkMesh::draw(Attrib *attrib) const {
    if(buffer){
        draw_triangles_3d_ao(attrib, buffer, faces * 6);
        return faces;
    } else {
        return 0;
    }
}

std::unique_ptr<ChunkMesh> ChunkMesh::set_dirty(bool dirty) const {
    return std::make_unique<ChunkMesh>(miny, maxy, faces, dirty, buffer, vertices);
}

std::unique_ptr<ChunkMesh> ChunkMesh::set_buffer(GLuint buffer) const{
    return std::make_unique<ChunkMesh>(miny, maxy, faces, dirty, buffer, vertices);
}

std::unique_ptr<ChunkMesh> ChunkMesh::set_vertices(const std::vector<GLfloat>& vertices) const {
    return std::make_unique<ChunkMesh>(miny, maxy, faces, dirty, buffer, vertices);
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