//
// Created by Nathanial Hartman on 3/13/17.
//

#include "WorldManager.h"
#include "../model.h"
#include "../util.h"
#include "./Actors.h"
#include "../chunk/ChunkMesh.h"

using namespace vgk;
using namespace actors;

extern Model *g;

static void set_dirty_flag(int p, int q);

static ChunkNeighbors find_neighbors(const Chunk& chunk){
    ChunkNeighbors neighbors;
    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            neighbors[std::make_tuple(dp + chunk.p, dq + chunk.q)] = g->find_chunk(dp + chunk.p, dq + chunk.q);
        }
    }
    return neighbors;
}

WorldManager::WorldManager(caf::actor_config &cfg)
        : caf::event_based_actor(cfg) {
}

caf::behavior WorldManager::make_behavior() {
    return {
        [&](wm_get_block, int x, int y, int z) {
            return g->get_block(x,y,z);
        },
        [&] (wm_set_block, int x, int y, int z, char w){
            auto p = chunked(x);
            auto q = chunked(z);
            auto chunk = g->find_chunk(p, q);
            g->update_chunk(p,q, [=](TransientChunk& chunk){
                if (chunk.get_block(x,y,z) != w) {
                    chunk.set_block(x, y, z, w);
                    set_dirty_flag(p, q);
                }
            });
            for(int dp = -1; dp <= 1; dp++){
                for(int dq = -1; dq <= 1; dq++){
                    auto chunk = g->find_chunk(p+dp, q+dq);
                    if(!chunk){
                        continue;
                    }
                    TransientChunkMesh mesh;
                    Chunk::create_mesh(chunk->p, chunk->q, mesh, *chunk->blocks, find_neighbors(*chunk));
                    g->replace_mesh(p+dp,q+dq, std::make_shared<ChunkMesh>(mesh.immutable()));
                }
            }
            return true;
        }
    };
}

static void set_dirty_flag(int p, int q) {
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            g->update_mesh(p + dp, q + dq, [&](TransientChunkMesh &mesh) {
                mesh.dirty = true;
            });
        }
    }
}
