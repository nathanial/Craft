//
// Created by nathan on 2/19/17.
//

#include "VisualChunk.h"
#include "../util.h"
#include "../draw.h"

VisualChunk::VisualChunk(int p, int q, int faces, int miny, int maxy, GLuint buffer, const std::vector<GLfloat>& vertices)
: p(p), q(q), faces(faces), miny(miny), maxy(maxy), buffer(buffer), vertices(vertices)
{

}

VisualChunk::~VisualChunk() {
    del_buffer(this->buffer);
}

int VisualChunk::draw(Attrib *attrib) const {
    if(this->buffer){
        draw_triangles_3d_ao(attrib, this->buffer, this->faces * 6);
        return this->faces;
    } else {
        return 0;
    }

}


VisualChunk generate_buffer(const VisualChunk &vchunk) {
    throw "Ooops";
//    if(this->_buffer) {
//        del_buffer(this->_buffer);
//    }
//    if(this->_vertices.size() == 0){
//        return;
//    }
//    this->_buffer = gen_buffer(this->vertices());
//    this->_vertices.clear();
}

int VisualChunk::distance(int p, int q) const {
    int dp = ABS(this->p - p);
    int dq = ABS(this->q - q);
    return MAX(dp, dq);
}

