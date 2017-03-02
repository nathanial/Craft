//
// Created by Nathanial Hartman on 3/2/17.
//

#include "ChunkRenderData.h"
#include "../draw.h"

int ChunkRenderData::draw(Attrib *attrib) const {
    if(buffer){
        draw_triangles_3d_ao(attrib, buffer, faces * 6);
        return faces;
    } else {
        return 0;
    }
}