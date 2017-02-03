//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/glew.h>
#include "block_map.h"
#include "height_map.h"
#include "sign.h"
#include <memory>
#include <mutex>
#include <shared_mutex>

class Attrib;

class Chunk {
private:
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;
    int _p, _q; // chunk position
public:

    int faces;
    bool dirty;
    int miny;
    int maxy;
    GLuint buffer;
    GLfloat *vertices;

    Chunk(int p, int q);
    ~Chunk();

    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    int set_block(int x, int y, int z, char w);

    int distance(int p, int q);
    void set_dirty_flag();

    void foreach_block(std::function<void (int, int, int, char)> func);
    void draw(Attrib *attrib);

    int p() const;
    int q() const;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

int highest_block(float x, float z);
int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
