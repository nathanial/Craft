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
            throw "Not Implemented";
        },
        [&] (wm_set_block, int x, int y, int z, char w){
            throw "Not Implemented";
        }
    };
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

std::vector<ChunkAndMesh> WorldManager::all_chunks() {
    caf::scoped_actor self { *vgk::actors::system };
    std::vector<ChunkAndMesh> result;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_all_chunks::value
    ).receive(
        [&](const std::vector<ChunkAndMesh> &chunks) {
            result = chunks;
        },
        [&](caf::error error){
            aout(self) << "Error: all_chunks " << error << std::endl;
            exit(0);
        }
    );
    return result;
}

int WorldManager::chunk_count() {
    caf::scoped_actor self { *vgk::actors::system };
    int result;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_count_chunks::value
    ).receive(
        [&](int count){
            result = count;
        },
        [&](caf::error error){
            aout(self) << "Error: count_chunks " << error << std::endl;
            exit(0);
        }
    );
    return result;
}

void WorldManager::load_world() {
    
}