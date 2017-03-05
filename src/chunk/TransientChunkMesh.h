//
// Created by Nathanial Hartman on 3/5/17.
//

#ifndef CRAFT_TRANSIENTCHUNKMESH_H
#define CRAFT_TRANSIENTCHUNKMESH_H


#include <GL/glew.h>
#include <vector>

class ChunkMesh;

class TransientChunkMesh {
public:
    int faces;
    bool dirty;
    int miny;
    int maxy;
    GLuint buffer;
    std::vector<GLfloat> vertices;

    std::shared_ptr<ChunkMesh> immutable() const;
};


#endif //CRAFT_TRANSIENTCHUNKMESH_H
