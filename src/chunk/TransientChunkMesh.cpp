//
// Created by Nathanial Hartman on 3/5/17.
//

#include "TransientChunkMesh.h"
#include "ChunkMesh.h"


std::shared_ptr<ChunkMesh> TransientChunkMesh::immutable() const {
    return std::make_shared<ChunkMesh>(
        miny, maxy, faces, dirty, buffer, vertices
    );
}