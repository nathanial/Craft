//
// Created by Nathanial Hartman on 3/13/17.
//

#include "World.h"
#include "../model.h"
#include "../util.h"
#include "./Actors.h"
#include "../chunk/ChunkMesh.h"
#include "../biomes/Mountains.h"

using namespace vgk;
using namespace actors;

extern Model *g;

World::World(caf::actor_config &cfg)
        : caf::event_based_actor(cfg) {
}

caf::behavior World::make_behavior() {
    return {
        [&](wm_get_block, int x, int y, int z) {
            return this->internal_get_block(x,y,z);
        },
        [&](wm_set_block, int x, int y, int z, char w){
            this->internal_set_block(x,y,z,w);
        },
        [&](wm_find_chunk_and_mesh, int p, int q) {
            ChunkAndMesh result = this->internal_find_chunk_and_mesh(p,q);
            return result;
        },
        [&](wm_find_neighbors, int p, int q) {
            return this->internal_find_neighbors(p,q);
        },
        [&](wm_update, int p, int q, const ChunkAndMesh& chunk_and_mesh){
            this->internal_update(p,q,chunk_and_mesh);
        },
        [&](wm_count_chunks) {
            return this->internal_count_chunks();
        },
        [&](wm_all_chunks) {
            return this->internal_all_chunks();
        },
        [&](wm_load_world) {
            this->internal_load_world();
            std::cout << "Load world complete" << std::endl;
        }
    };
}

static caf::strong_actor_ptr get_world_manager() {
    return vgk::actors::system->registry().get(vgk::actors::world_manager_id::value);
}

ChunkAndMesh World::find(int p, int q) {
    caf::scoped_actor self { *vgk::actors::system };
    ChunkAndMesh result(nullptr, nullptr);
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_find_chunk_and_mesh::value, p, q
    ).receive(
        [&](const ChunkAndMesh &cm){
            result = cm;
        },
        [&](caf::error error){
            caf::aout(self) << "Error with find: " << error << std::endl;
            exit(0);
        }
    );
    return result;
}

ChunkNeighbors World::find_neighbors(int p, int q) {
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

void World::update(int p, int q, const ChunkAndMesh& mesh) {
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

char World::get_block(int x, int y, int z) {
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

void World::set_block(int x, int y, int z, char w) {
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

std::vector<ChunkAndMesh> World::all_chunks() {
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

int World::chunk_count() {
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

void World::load_world() {
    caf::scoped_actor self { *vgk::actors::system };
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_load_world::value
    ).receive(
        [&]() {
        },
        [&](caf::error error) {
            aout(self) << "Error: load_world " << error << std::endl;
            exit(0);
        }
    );
}

char World::internal_get_block(int x, int y, int z) {
    throw "Not Implemented";
}

void World::internal_set_block(int x, int y, int z, char w) {
    throw "Not Implemented";
}

ChunkAndMesh World::internal_find_chunk_and_mesh(int p, int q) {
    return this->chunks[std::make_tuple(p,q)];
}

ChunkNeighbors World::internal_find_neighbors(int p, int q) {
    ChunkNeighbors results;
    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            auto position = std::make_tuple(p+dp,q+dq);
            results[position] = chunks[position].chunk;
        }
    }
    return results;
}

void World::internal_update(int p, int q, const ChunkAndMesh& chunk_and_mesh) {
    throw "Not Implemented";
}

int World::internal_count_chunks() {
    return static_cast<int>(this->chunks.size());
}

ChunksAndMeshes World::internal_all_chunks() {
    ChunksAndMeshes result;
    for(auto& entry : this->chunks){
        result.push_back(entry.second);
    }
    return result;
}

void World::internal_load_world() {
    for(int p = -10; p < 10; p++){
        for(int q = -10; q < 10; q++){
            Mountains mountains;
            TransientChunk chunk(p,q);
            mountains.create_chunk(chunk, p,q);
            chunks[std::make_tuple(p,q)] = ChunkAndMesh(std::make_shared<Chunk>(chunk.immutable()), nullptr);
        }
    }
    for(int p = -10; p < 10; p++) {
        for (int q = -10; q < 10; q++) {
            auto chunk = chunks[std::make_tuple(p,q)].chunk;
            TransientChunkMesh mesh;
            Chunk::create_mesh(p, q, mesh, *chunk->blocks, this->internal_find_neighbors(p, q));
            chunks[std::make_tuple(p,q)] = ChunkAndMesh(
                chunk, std::make_shared<ChunkMesh>(mesh.immutable())
            );
        }
    }
}