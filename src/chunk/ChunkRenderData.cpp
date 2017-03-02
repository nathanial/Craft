//
// Created by Nathanial Hartman on 3/2/17.
//

#include "ChunkRenderData.h"
#include "../draw.h"
#include "../util.h"

int ChunkRenderData::draw(Attrib *attrib) const {
    if(buffer){
        draw_triangles_3d_ao(attrib, buffer, faces * 6);
        return faces;
    } else {
        return 0;
    }
}

std::shared_ptr<ChunkRenderData> ChunkRenderData::generate_buffer() const {
    if(buffer) {
        del_buffer(buffer);
        if(vertices.size() == 0){
            return set_buffer(0);
        }
    }
    std::vector<GLfloat> emptyVertices;
    return this->set_buffer(gen_buffer(vertices))
            ->set_vertices(emptyVertices);
}
