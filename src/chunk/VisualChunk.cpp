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
    if(this->buffer){
        del_buffer(this->buffer);
    }
}

int VisualChunk::draw(Attrib *attrib) const {
    if(this->buffer){
        draw_triangles_3d_ao(attrib, this->buffer, this->faces * 6);
        return this->faces;
    } else {
        printf("SKIP DRAW %d,%d\n", this->p, this->q);
        return 0;
    }

}


VisualChunkPtr generate_buffer(const VisualChunk &vchunk) {
    return std::make_shared<VisualChunk>(
        vchunk.p,
        vchunk.q,
        vchunk.faces,
        vchunk.miny,
        vchunk.maxy,
        gen_buffer(vchunk.vertices),
        vchunk.vertices
    );
}

int VisualChunk::distance(int p, int q) const {
    int dp = ABS(this->p - p);
    int dq = ABS(this->q - q);
    return MAX(dp, dq);
}

