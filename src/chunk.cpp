//
// Created by nathan on 1/17/17.
//

#include "chunk.h"
#include "db.h"
#include "model.h"
#include "util.h"
#include "item.h"
#include "draw.h"

extern Model *g;


Chunk::Chunk(int p, int q) :
    blocks(new BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>())
{
    this->p = p;
    this->q = q;
    this->faces = 0;
    this->buffer = 0;
    this->set_dirty_flag();
}

Chunk::~Chunk() {
    del_buffer(this->buffer);
}

int Chunk::get_block(int x, int y, int z) const {
    return this->blocks->get(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE);
}

int Chunk::get_block_or_zero(int x, int y, int z) const {
    return this->blocks->get_or_default(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE, 0);
}

void Chunk::foreach_block(std::function<void (int, int, int, char)> func) {
    this->blocks->each([&](int x, int y, int z, char w){
        func(x + this->p * CHUNK_SIZE, y, z + this->q * CHUNK_SIZE, w);
    });
}

int Chunk::set_block(int x, int y, int z, char w){
    return this->blocks->set(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE, w);
}

void Chunk::set_dirty_flag() {
    this->dirty = 1;
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            auto other = g->find_chunk(this->p + dp, this->q + dq);
            if (other) {
                other->dirty = 1;
            }
        }
    }
}

int Chunk::distance(int p, int q) {
    int dp = ABS(this->p - p);
    int dq = ABS(this->q - q);
    return MAX(dp, dq);
}

void Chunk::draw(Attrib *attrib) {
    draw_triangles_3d_ao(attrib, this->buffer, this->faces * 6);
}

int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy) {
    int x = p * CHUNK_SIZE - 1;
    int z = q * CHUNK_SIZE - 1;
    int d = CHUNK_SIZE + 1;
    float points[8][3] = {
            {x + 0, miny, z + 0},
            {x + d, miny, z + 0},
            {x + 0, miny, z + d},
            {x + d, miny, z + d},
            {x + 0, maxy, z + 0},
            {x + d, maxy, z + 0},
            {x + 0, maxy, z + d},
            {x + d, maxy, z + d}
    };
    int n = g->ortho ? 4 : 6;
    for (int i = 0; i < n; i++) {
        int in = 0;
        int out = 0;
        for (int j = 0; j < 8; j++) {
            float d =
                    planes[i][0] * points[j][0] +
                    planes[i][1] * points[j][1] +
                    planes[i][2] * points[j][2] +
                    planes[i][3];
            if (d < 0) {
                out++;
            }
            else {
                in++;
            }
            if (in && out) {
                break;
            }
        }
        if (in == 0) {
            return 0;
        }
    }
    return 1;
}

int highest_block(float x, float z) {
    int result = -1;
    int nx = roundf(x);
    int nz = roundf(z);
    int p = chunked(x);
    int q = chunked(z);
    printf("Highest Block %f %f\n", x, z);
    auto chunk = g->find_chunk(p, q);
    if (chunk) {
        chunk->foreach_block([&](int ex, int ey, int ez, int ew){
            if (is_obstacle(ew) && ex == nx && ez == nz) {
                result = MAX(result, ey);
            }
        });
    }
    return result;
}
