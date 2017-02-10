//
// Created by nathan on 1/17/17.
//

extern "C" {
    #include <noise.h>
}

#include <queue>
#include "chunk.h"
#include "../util.h"
#include "../item.h"
#include "../matrix.h"
#include "../draw.h"

void occlusion(
        char neighbors[27], char lights[27], float shades[27],
        float ao[6][4], float light[6][4]);

void make_plant(
        float *data, float ao, float light,
        float px, float py, float pz, float n, int w, float rotation);

void make_cube(
        float *data, float ao[6][4], float light[6][4],
        int left, int right, int top, int bottom, int front, int back,
        float x, float y, float z, float n, int w);

void scanline_iterate(BigBlockMap *light, BigBlockMap *opaque, std::deque<std::tuple<int, int, int, int>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, bool ascend);


void light_fill_scanline(BigBlockMap *opaque, BigBlockMap *light, int ox, int oy ,int oz, int ow);

Chunk::Chunk(int p, int q) :
    blocks(new BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>())
{
    this->_p = p;
    this->_q = q;
}

Chunk::~Chunk() {
}

int Chunk::get_block(int x, int y, int z) const {
    return this->blocks->get(x - this->_p * CHUNK_SIZE, y, z - this->_q * CHUNK_SIZE);
}

int Chunk::get_block_raw(int x, int y, int z) const {
    return this->blocks->get(x, y, z);
}

int Chunk::get_block_or_zero(int x, int y, int z) const {
    return this->blocks->get_or_default(x - this->_p * CHUNK_SIZE, y, z - this->_q * CHUNK_SIZE, 0);
}

void Chunk::foreach_block(std::function<void (int, int, int, char)> func) const {
    this->blocks->each([&](int x, int y, int z, char w){
        func(x + this->_p * CHUNK_SIZE, y, z + this->_q * CHUNK_SIZE, w);
    });
}

int Chunk::set_block(int x, int y, int z, char w){
    return this->blocks->set(x - this->_p * CHUNK_SIZE, y, z - this->_q * CHUNK_SIZE, w);
}

int Chunk::distance(int p, int q) {
    int dp = ABS(this->_p - p);
    int dq = ABS(this->_q - q);
    return MAX(dp, dq);
}


int Chunk::p() const {
    return this->_p;
}

int Chunk::q() const {
    return this->_q;
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
    int n = 6;
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

