//
// Created by Nathanial Hartman on 3/2/17.
//

#ifndef CRAFT_CHUNKRENDERDATA_H
#define CRAFT_CHUNKRENDERDATA_H


#include <GL/glew.h>
#include <vector>

class Attrib;

class ChunkRenderData {
public:
    const int faces;
    const bool dirty;
    const int miny;
    const int maxy;
    const GLuint buffer;
    const std::vector<GLfloat> vertices;

    ChunkRenderData() : faces(0), buffer(0), dirty(false), miny(INT_MAX), maxy(INT_MIN) {}
    ChunkRenderData(int miny, int maxy, int faces, bool dirty, GLuint buffer, const std::vector<GLfloat> &vertices) :
            miny(miny), maxy(maxy), faces(faces), dirty(dirty), buffer(buffer), vertices(vertices) {}

    std::unique_ptr<ChunkRenderData> set_dirty(bool dirty) const;
    std::unique_ptr<ChunkRenderData> set_buffer(GLuint buffer) const;
    std::unique_ptr<ChunkRenderData> set_vertices(const std::vector<GLfloat>& vertices) const;
    std::unique_ptr<ChunkRenderData> generate_buffer() const;
    int draw(Attrib *attrib) const;
};


#endif //CRAFT_CHUNKRENDERDATA_H
