//
// Created by Nathanial Hartman on 3/13/17.
//

#include "WorldManager.h"
#include "../model.h"
#include "../util.h"
#include "../db.h"
#include "./Actors.h"
#include "../chunk/ChunkMesh.h"

using namespace vgk;
using namespace actors;

extern Model *g;

static void set_dirty_flag(int p, int q);

WorldManager::WorldManager(caf::actor_config &cfg)
        : caf::event_based_actor(cfg) {
}

caf::behavior WorldManager::make_behavior() {
    return {
        [&](wm_get_block, int x, int y, int z) {
            return g->get_block(x,y,z);
        },
        [&] (wm_set_block, int x, int y, int z, char w){
            auto chunk_mesher = caf::actor_cast<caf::actor>(vgk::actors::system->registry().get(chunk_mesher_id::value));
            auto p = chunked(x);
            auto q = chunked(z);
            auto chunk = g->find_chunk(p, q);
            g->update_chunk(p,q, [=](TransientChunk& chunk){
                if (chunk.get_block(x,y,z) != w) {
                    chunk.set_block(x, y, z, w);
                    set_dirty_flag(p, q);
                    db_insert_block(p, q, x, y, z, w);
                }
            });

            auto result = this->make_response_promise<bool>();
            for(int dp = -1; dp <= 1; dp++){
                for(int dq = -1; dq <= 1; dq++){
                    auto chunk = g->find_chunk(p+dp, q+dq);
                    this->request(chunk_mesher, caf::infinite, *chunk).await(
                        [=](const ChunkMesh& mesh) mutable {
                            g->replace_mesh(p+dp,q+dq, std::make_shared<ChunkMesh>(mesh));
                            if(dp == 1 && dq == 1){
                                result.deliver(true);
                            }
                        }
                    );
                }
            }
            return result;
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
