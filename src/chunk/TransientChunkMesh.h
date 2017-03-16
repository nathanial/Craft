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
    int faces = 0;
    int miny = INT_MAX;
    int maxy = INT_MIN;
    GLuint buffer = 0;
    std::vector<GLfloat> vertices = {};

    ChunkMesh immutable() const;
    void generate_buffer();


};


#endif //CRAFT_TRANSIENTCHUNKMESH_H
