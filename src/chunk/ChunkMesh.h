//
// Created by Nathanial Hartman on 3/2/17.
//

#ifndef CRAFT_CHUNKRENDERDATA_H
#define CRAFT_CHUNKRENDERDATA_H


#include <GL/glew.h>
#include <vector>
#include <caf/allowed_unsafe_message_type.hpp>

class Attrib;
class TransientChunkMesh;

class ChunkMesh {
public:
    const int faces;
    const bool dirty;
    const int miny;
    const int maxy;
    const GLuint buffer;
    const std::vector<GLfloat> vertices;

    ChunkMesh() : faces(0), buffer(0), dirty(false), miny(INT_MAX), maxy(INT_MIN) {}
    ChunkMesh(int miny, int maxy, int faces, bool dirty, GLuint buffer, const std::vector<GLfloat> &vertices) :
            miny(miny), maxy(maxy), faces(faces), dirty(dirty), buffer(buffer), vertices(vertices) {}

    int draw(Attrib *attrib) const;

    bool is_ready_to_draw() const;

    std::shared_ptr<TransientChunkMesh> transient() const;
};


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkMesh);

#endif //CRAFT_CHUNKRENDERDATA_H
