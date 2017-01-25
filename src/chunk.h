//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/glew.h>
#include "map.h"
#include "sign.h"

class Chunk {
public:
    Map *lights;
    Map *blocks;

    SignList signs;
    int p, q; // chunk position
    int faces;
    int sign_faces;
    bool dirty;
    int miny;
    int maxy;
    GLuint buffer;
    GLuint sign_buffer;

    void init(int p, int q);
    int get_block(int x, int y, int z) const;
    int get_light(int x, int y, int z) const;
    int set_block(int x, int y, int z, char w);
    int set_light(int x, int y, int z, char w);
    void foreach_block(std::function<void (int, int, int, char)> func);
    int distance(int p, int q);
    int has_lights();
    void set_dirty_flag();
};

Chunk *find_chunk(int p, int q);
int highest_block(float x, float z);
int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
