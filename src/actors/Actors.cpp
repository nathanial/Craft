//
// Created by nathan on 1/21/17.
//

#include <caf/all.hpp>
#include "../chunk/chunk.h"
#include "../biomes/mountains.h"
#include "../model.h"
#include "../chunk/ChunkMesh.h"
#include <future>
#include "./Actors.h"
#include "WorldManager.h"

using namespace vgk;
using namespace vgk::actors;

extern Model *g;

caf::actor_system_config vgk::actors::cfg;
caf::actor_system *vgk::actors::system;

static ChunkNeighbors find_neighbors(const Chunk& chunk){
    ChunkNeighbors neighbors;
    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            neighbors[std::make_tuple(dp + chunk.p, dq + chunk.q)] = g->find_chunk(dp + chunk.p, dq + chunk.q);
        }
    }
    return neighbors;
}

caf::behavior chunk_builder(caf::event_based_actor* self) {
    // return the (initial) actor behavior
    return {
            // a handler for messages containing a single string
            // that replies with a string
            [=](create_chunk, int p, int q) -> Chunk {
                Mountains mountains;
                TransientChunk chunk(p,q);
                mountains.create_chunk(chunk, p, q);
                return chunk.immutable();
            }
    };
}

caf::behavior chunk_mesher(caf::event_based_actor* self) {
    return {
      [=](const Chunk& chunk) -> ChunkMesh {
          TransientChunkMesh mesh;
          Chunk::create_mesh(chunk.p, chunk.q, mesh, *chunk.blocks, find_neighbors(chunk));
          return mesh.immutable();
      }
    };
}

caf::behavior world_builder(caf::event_based_actor* self) {
    return {
        [=](load_nearby_chunks) {
            auto chunk_builder = caf::actor_cast<caf::actor>(vgk::actors::system->registry().get(chunk_builder_id::value));
            auto chunk_mesher = caf::actor_cast<caf::actor>(vgk::actors::system->registry().get(chunk_mesher_id::value));

            auto result = self->make_response_promise<int>();
            for(int p = -10; p < 10; p++){
                for(int q = -10; q < 10; q++){
                    self->request(chunk_builder, caf::infinite, create_chunk::value, p, q).then(
                            [=](const Chunk& chunk) mutable {
                                // ... and print it
                                caf::aout(self) << "Created Chunk " << chunk.p << "," << chunk.q << std::endl;
                                g->replace_chunk(std::make_shared<Chunk>(chunk));
                                self->request(chunk_mesher, caf::infinite, chunk).then(
                                        [=](const ChunkMesh& mesh) mutable {
                                            caf::aout(self) << "Meshed Chunk " << chunk.p << "," << chunk.q << std::endl;
                                            g->replace_mesh(p,q, std::make_shared<ChunkMesh>(mesh));
                                            if(p == 9 && q == 9){
                                                result.deliver(0);
                                            }
                                        }
                                );
                            }
                    );
                }
            }
            return result;
        }
    };
}


void vgk::actors::start(){
    vgk::actors::system = new caf::actor_system { vgk::actors::cfg };

    auto chunk_builder_actor = vgk::actors::system->spawn(chunk_builder);
    auto chunk_mesher_actor = vgk::actors::system->spawn(chunk_mesher);
    auto world_manager_actor = vgk::actors::system->spawn<WorldManager>();

    auto actor_ptr = caf::actor_cast<caf::strong_actor_ptr>(world_manager_actor);

    vgk::actors::system->registry().put(chunk_builder_id::value,
                                       caf::actor_cast<caf::strong_actor_ptr>(chunk_builder_actor));
    vgk::actors::system->registry().put(chunk_mesher_id::value,
                                       caf::actor_cast<caf::strong_actor_ptr>(chunk_mesher_actor));

    vgk::actors::system->registry().put(world_manager_id::value,actor_ptr);

    auto world_builder_actor = vgk::actors::system->spawn(world_builder);
    caf::scoped_actor self { *vgk::actors::system };
    self->request(world_builder_actor, caf::infinite, load_nearby_chunks::value).receive(
        [&](int result){},
        [&](caf::error error){
            std::cout << "Error" << std::endl;
        }
    );
}

