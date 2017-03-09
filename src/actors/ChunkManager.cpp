//
// Created by Nathanial Hartman on 3/9/17.
//

#include "ChunkManager.h"
#include "../util.h"

static ChunkNeighbors find_neighbors(ChunkMap& allChunks, int p, int q){
    ChunkNeighbors neighbors;
    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            auto position = std::make_tuple(dp + p, dq + q);
            if(allChunks.count(position) > 0){
                neighbors[position] = allChunks.at(position);
            } else {
                neighbors[position] = nullptr;
            }
        }
    }
    return neighbors;
}

static int _hit_test(
        Chunk& chunk, float max_distance, int previous,
        float x, float y, float z,
        float vx, float vy, float vz,
        int *hx, int *hy, int *hz)
{
    int m = 32;
    int px = 0;
    int py = 0;
    int pz = 0;
    for (int i = 0; i < max_distance * m; i++) {
        int nx = roundf(x);
        int ny = roundf(y);
        int nz = roundf(z);
        if (nx != px || ny != py || nz != pz) {
            int hw = chunk.get_block_or_zero(nx, ny, nz);
            if (hw > 0) {
                if (previous) {
                    *hx = px; *hy = py; *hz = pz;
                }
                else {
                    *hx = nx; *hy = ny; *hz = nz;
                }
                return hw;
            }
            px = nx; py = ny; pz = nz;
        }
        x += vx / m; y += vy / m; z += vz / m;
    }
    return 0;
}


ChunkManager::ChunkManager(caf::actor_config& cfg) : caf::event_based_actor(cfg) {
}

caf::behavior ChunkManager::make_behavior() {
    return {
            [&](chunk_manager_add, ChunkPtr val){
                this->_chunks[std::make_tuple(val->p, val->q)] = val;
            },
            [&](chunk_manager_remove, int p, int q) {
                this->_chunks.erase(std::make_tuple(p,q));
            },
            [&](chunk_manager_find_neighborhood, int p, int q){
                return find_neighbors(this->_chunks, p, q);
            },
            [&](chunk_manager_add_mesh, int p, int q, const ChunkMesh &mesh){
                this->_meshes[std::make_tuple(p,q)] = std::make_shared<ChunkMesh>(mesh);
            }
    };
}


int ChunkManager::hit_test(
        int previous, float x, float y, float z, float rx, float ry,
        int *bx, int *by, int *bz)
{
    int result = 0;
    float best = 0;
    int p = chunked(x);
    int q = chunked(z);
    float vx, vy, vz;
    get_sight_vector(rx, ry, &vx, &vy, &vz);
    for(auto &kv : this->_chunks){
        auto chunk = kv.second;
        if (chunk->distance(p, q) > 1) {
            continue;
        }
        int hx, hy, hz;
        int hw = _hit_test(chunk, 8, previous,
                           x, y, z, vx, vy, vz, &hx, &hy, &hz);
        if (hw > 0) {
            float d = sqrtf(
                    powf(hx - x, 2) + powf(hy - y, 2) + powf(hz - z, 2));
            if (best == 0 || d < best) {
                best = d;
                *bx = hx; *by = hy; *bz = hz;
                result = hw;
            }
        }
    });
    return result;
}

int ChunkManager::hit_test_face(Player *player, int *x, int *y, int *z, int *face) {
    State *s = &player->state;
    int w = hit_test(0, s->x, s->y, s->z, s->rx, s->ry, x, y, z);
    if (is_obstacle(w)) {
        int hx, hy, hz;
        hit_test(1, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz);
        int dx = hx - *x;
        int dy = hy - *y;
        int dz = hz - *z;
        if (dx == -1 && dy == 0 && dz == 0) {
            *face = 0; return 1;
        }
        if (dx == 1 && dy == 0 && dz == 0) {
            *face = 1; return 1;
        }
        if (dx == 0 && dy == 0 && dz == -1) {
            *face = 2; return 1;
        }
        if (dx == 0 && dy == 0 && dz == 1) {
            *face = 3; return 1;
        }
        if (dx == 0 && dy == 1 && dz == 0) {
            int degrees = roundf(DEGREES(atan2f(s->x - hx, s->z - hz)));
            if (degrees < 0) {
                degrees += 360;
            }
            int top = ((degrees + 45) / 90) % 4;
            *face = 4 + top; return 1;
        }
    }
    return 0;
}