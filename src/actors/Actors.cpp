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
      [=](const Chunk& chunk, const ChunkNeighbors& neighbors) -> ChunkMesh {
          TransientChunkMesh mesh;
          Chunk::create_mesh(chunk.p, chunk.q, mesh, *chunk.blocks, neighbors);
          return mesh.immutable();
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

    WorldManager::load_world();
}

