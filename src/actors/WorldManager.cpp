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
                }
            });

            auto result = this->make_response_promise<bool>();
            for(int dp = -1; dp <= 1; dp++){
                for(int dq = -1; dq <= 1; dq++){
                    auto chunk = g->find_chunk(p+dp, q+dq);
                    if(!chunk){
                        continue;
                    }
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

static caf::strong_actor_ptr get_world_manager() {
    return vgk::actors::system->registry().get(vgk::actors::world_manager_id::value);
}

ChunkAndMesh WorldManager::find(int p, int q) {
    caf::scoped_actor self { *vgk::actors::system };
    ChunkAndMesh result = nullptr;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_find_chunk_and_mesh::value, p, q
    ).receive(
        [&](const ChunkAndMesh &cm){
            result = cm;
        },
        [&](caf::error error){
            caf::aout(self) << "Error " << error << std::endl;
            exit(0);
        }
    );
    return result;
}

ChunkNeighbors WorldManager::find_neighbors(int p, int q) {
    caf::scoped_actor self { *vgk::actors::system };
    ChunkNeighbors result;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_find_neighbors::value,
        p, q
    ).receive(
        [&](const ChunkNeighbors &neighbors){
            result = neighbors;
        },
        [&](caf::error error){
            aout(self) << "Error: wm_find_neighbors" << error << std::endl;
            exit(0);
        }
    );
    return result;
}

void WorldManager::update(int p, int q, const ChunkAndMesh& mesh) {
    caf::scoped_actor self { *vgk::actors::system };
    self->request(
            caf::actor_cast<caf::actor>(get_world_manager()),
            caf::infinite,
            vgk::actors::wm_update::value,
            p, q, mesh
    ).receive(
        [&](bool _){
        },
        [&](caf::error error){
            aout(self) << "Error: update " << error << std::endl;
            exit(0);
        }
    );
}

char WorldManager::get_block(int x, int y, int z) {
    caf::scoped_actor self { *vgk::actors::system };
    char result;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_get_block::value,
        x,y,z
    ).receive(
        [&](char w){
            result = w;
        },
        [&](caf::error error){
            aout(self) << "Error: get_block " << error << std::endl;
            exit(0);
        }
    );
    return result;
}

void WorldManager::set_block(int x, int y, int z, char w) {
    caf::scoped_actor self { *vgk::actors::system };
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_set_block::value,
        x,y,z,w
    ).receive(
        [&](bool _){

        },
        [&](caf::error error){
            aout(self) << "Error: set_block " << error << std::endl;
            exit(0);
        }
    );
}
