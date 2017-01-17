//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/gl.h>
#include "map.h"
#include "sign.h"

typedef struct {
    Map map;
    Map lights;
    SignList signs;
    int p, q; // chunk position
    int faces;
    int sign_faces;
    int dirty;
    int miny;
    int maxy;
    GLuint buffer;
    GLuint sign_buffer;
} Chunk;

void init_chunk(Chunk *chunk, int p, int q);
void dirty_chunk(Chunk *chunk);
int has_lights(Chunk *chunk);
Chunk *find_chunk(int p, int q);

#endif //CRAFT_CHUNK_H
