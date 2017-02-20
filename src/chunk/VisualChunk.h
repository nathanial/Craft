//
// Created by nathan on 2/19/17.
//

#ifndef CRAFT_VISUALCHUNK_H
#define CRAFT_VISUALCHUNK_H

#include <GL/glew.h>
#include <vector>
#include <memory>

class Attrib;

class VisualChunk {
public:
    const int p;
    const int q;
    const int faces;
    const int miny;
    const int maxy;
    const GLuint buffer;
    const std::vector<GLfloat> vertices;

    VisualChunk(int p, int q, int faces, int miny, int maxy, GLuint buffer, const std::vector<GLfloat>& vertices);
    ~VisualChunk();

    int draw(Attrib *attrib) const;
    int distance(int p, int q) const;
};

typedef std::shared_ptr<VisualChunk> VisualChunkPtr;

VisualChunk generate_buffer(const VisualChunk &chunk);


#endif //CRAFT_VISUALCHUNK_H
