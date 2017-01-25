//
// Created by nathan on 1/17/17.
//

#include "chunk.h"
#include "config.h"
#include "db.h"
#include "model.h"
#include "util.h"
#include "item.h"

extern Model *g;

void Chunk::init(int p, int q) {
    this->p = p;
    this->q = q;
    this->faces = 0;
    this->sign_faces = 0;
    this->buffer = 0;
    this->sign_buffer = 0;

    int dx = p * CHUNK_SIZE;
    int dy = 0;
    int dz = q * CHUNK_SIZE;

    this->blocks = new Map(dx,dy,dz);
    this->lights = new Map(dx,dy,dz);

    this->set_dirty_flag();
    SignList *signs = &this->signs;
    sign_list_alloc(signs, 16);
    db_load_signs(signs, p, q);

}

int Chunk::get_block(int x, int y, int z) const {
    return this->blocks->get(x, y, z);
}

int Chunk::get_light(int x, int y, int z) const {
    return this->lights->get(x, y, z);
}

int Chunk::set_light(int x, int y, int z, char w) {
    return this->lights->set(x, y, z, w);
}

void Chunk::foreach_block(std::function<void (int, int, int, char)> func) {
    this->blocks->each(func);
}

int Chunk::set_block(int x, int y, int z, char w){
    return this->blocks->set(x, y, z, w);
}


void Chunk::set_dirty_flag() {
    this->dirty = 1;
    if (this->has_lights()) {
        for (int dp = -1; dp <= 1; dp++) {
            for (int dq = -1; dq <= 1; dq++) {
                Chunk *other = find_chunk(this->p + dp, this->q + dq);
                if (other) {
                    other->dirty = 1;
                }
            }
        }
    }
}

int Chunk::has_lights() {
    if (!SHOW_LIGHTS) {
        return 0;
    }
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = this;
            if (dp || dq) {
                other = find_chunk(this->p + dp, this->q + dq);
            }
            if (!other) {
                continue;
            }
            Map *map = other->lights;
            if (map->size()) {
                return 1;
            }
        }
    }
    return 0;
}

Chunk *find_chunk(int p, int q) {
    for (int i = 0; i < g->chunk_count; i++) {
        Chunk *chunk = g->chunks + i;
        if (chunk->p == p && chunk->q == q) {
            return chunk;
        }
    }
    return 0;
}


int Chunk::distance(int p, int q) {
    int dp = ABS(this->p - p);
    int dq = ABS(this->q - q);
    return MAX(dp, dq);
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
    Chunk *chunk = find_chunk(p, q);
    if (chunk) {
        chunk->foreach_block([&](int ex, int ey, int ez, int ew){
            if (is_obstacle(ew) && ex == nx && ez == nz) {
                result = MAX(result, ey);
            }
        });
    }
    return result;
}
