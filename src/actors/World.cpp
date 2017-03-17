//
// Created by Nathanial Hartman on 3/13/17.
//

#include "World.h"
#include "../model.h"
#include "../util.h"
#include "./Actors.h"
#include "../chunk/ChunkMesh.h"
#include "../biomes/Mountains.h"
#include "../biomes/flatland.h"

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
            VisualChunkPtr result = this->internal_find_visual_chunk(p, q);
            return result;
        },
        [&](wm_find_neighbors, int p, int q) {
            return this->internal_find_neighbors(p,q);
        },
        [&](wm_update, int p, int q, const VisualChunk& chunk_and_mesh){
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

VisualChunkPtr World::find(int p, int q) {
    caf::scoped_actor self { *vgk::actors::system };
    VisualChunkPtr result;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_find_chunk_and_mesh::value, p, q
    ).receive(
        [&](const VisualChunkPtr cm){
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

void World::update(int p, int q, const VisualChunk& mesh) {
    caf::scoped_actor self { *vgk::actors::system };
    self->request(
            caf::actor_cast<caf::actor>(get_world_manager()),
            caf::infinite,
            vgk::actors::wm_update::value,
            p, q, mesh
    ).receive(
        [&](){
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
        [&](){
            std::cout << "COMPLETE" << std::endl;
        },
        [&](caf::error error){
            aout(self) << "Error: set_block " << error << std::endl;
            exit(0);
        }
    );
}

std::vector<VisualChunkPtr> World::all_chunks() {
    caf::scoped_actor self { *vgk::actors::system };
    std::vector<VisualChunkPtr> result;
    self->request(
        caf::actor_cast<caf::actor>(get_world_manager()),
        caf::infinite,
        vgk::actors::wm_all_chunks::value
    ).receive(
        [&](const std::vector<VisualChunkPtr> &chunks) {
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
    int p = chunked(x);
    int q = chunked(z);
    auto cm = this->internal_find_visual_chunk(p, q);
    if(!cm){
        return 0;
    }
    auto chunk = cm->chunk;
    if(!chunk){
        return 0;
    }
    if(y < 0){
        return 0;
    }
    return chunk->get_block(x,y,z);
}

void World::internal_set_block(int x, int y, int z, char w) {
    auto p = chunked(x);
    auto q = chunked(z);
    auto vchunk = this->internal_find_visual_chunk(p, q);
    if(!vchunk || !vchunk->chunk){
        return;
    }

    auto updated_chunk = vchunk->chunk->transient();
    if (updated_chunk->get_block(x,y,z) != w) {
        updated_chunk->set_block(x, y, z, w);
    }
    this->visual_chunks[std::make_tuple(p,q)] = std::make_shared<VisualChunk>(
            std::make_shared<Chunk>(updated_chunk->immutable()),
            vchunk->mesh
    );

    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            auto vchunk = this->internal_find_visual_chunk(p + dp, q + dq);
            if(!vchunk){
                continue;
            }
            TransientChunkMesh mesh;
            Chunk::create_mesh(p+dp,q+dq, mesh, *vchunk->chunk->blocks, this->internal_find_neighbors(p+dp,q+dq));
            this->visual_chunks[std::make_tuple(p+dp,q+dq)] = std::make_shared<VisualChunk>(
                    vchunk->chunk,
                    std::make_shared<ChunkMesh>(mesh.immutable())
            );
        }
    }
}

VisualChunkPtr World::internal_find_visual_chunk(int p, int q) {
    return this->visual_chunks[std::make_tuple(p,q)];
}

ChunkNeighbors World::internal_find_neighbors(int p, int q) {
    ChunkNeighbors results;
    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            auto position = std::make_tuple(p+dp,q+dq);
            auto cm = visual_chunks[position];
            if(cm){
                results[position] = visual_chunks[position]->chunk;
            } else {
                results[position] = nullptr;
            }
        }
    }
    return results;
}

void World::internal_update(int p, int q, const VisualChunk& chunk_and_mesh) {
    this->visual_chunks[std::make_tuple(p,q)] = std::make_shared<VisualChunk>(chunk_and_mesh.chunk, chunk_and_mesh.mesh);
}

int World::internal_count_chunks() {
    return static_cast<int>(this->visual_chunks.size());
}

VisualChunks World::internal_all_chunks() {
    VisualChunks result;
    for(auto& entry : this->visual_chunks){
        if(entry.second){
            result.push_back(entry.second);
        }
    }
    return result;
}

void World::internal_load_world() {
    std::clock_t    start;
    start = std::clock();

    int min = 0;
    int max = 2;

    for(int p = min; p < max; p++){
        for(int q = min; q < max; q++){
            Mountains flatland;
            TransientChunk chunk(p,q);
            flatland.create_chunk(chunk, p,q);
            visual_chunks[std::make_tuple(p,q)] = std::make_shared<VisualChunk>(std::make_shared<Chunk>(chunk.immutable()), nullptr);
        }
    }
    for(int p = min; p < max; p++) {
        for (int q = min; q < max; q++) {
            auto chunk = visual_chunks[std::make_tuple(p,q)]->chunk;
            TransientChunkMesh mesh;
            Chunk::create_mesh(p, q, mesh, *chunk->blocks, this->internal_find_neighbors(p, q));
            visual_chunks[std::make_tuple(p,q)] = std::make_shared<VisualChunk>(
                chunk, std::make_shared<ChunkMesh>(mesh.immutable())
            );
        }
    }

    std::cout << "Load World Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
}