//
// Created by nathan on 1/21/17.
//

#include <caf/all.hpp>
#include "chunk/chunk.h"
#include "biomes/mountains.h"
#include "model.h"
#include "chunk/ChunkMesh.h"
#include "actors/ChunkManager.h"

using create_chunk = caf::atom_constant<caf::atom("create")>;

caf::behavior chunk_builder(caf::event_based_actor* self) {
    // return the (initial) actor behavior
    return {
            // a handler for messages containing a single string
            // that replies with a string
            [=](create_chunk, int p, int q) -> ChunkPtr {
                Mountains mountains;
                TransientChunk chunk(p,q);
                mountains.create_chunk(chunk, p, q);
                return std::make_shared<Chunk>(chunk.immutable());
            }
    };
}

caf::behavior chunk_mesher(caf::event_based_actor* self) {
    return {
      [=](ChunkPtr chunk, const ChunkNeighbors& neighbors) -> ChunkMesh {
          TransientChunkMesh mesh;
          Chunk::create_mesh(chunk->p, chunk->q, mesh, *chunk->blocks, neighbors);
          return mesh.immutable();
      }
    };
}


void build_world(caf::event_based_actor* self,
                 const caf::actor& chunk_builder,
                 const caf::actor& chunk_mesher,
                 const caf::actor& chunk_manager) {
    for(int p = -10; p < 10; p++){
        for(int q = -10; q < 10; q++){
            self->request(chunk_builder, std::chrono::seconds(10), create_chunk::value, p, q).then(
                [=](ChunkPtr chunk) {
                    // ... and print it
                    caf::aout(self) << "Created Chunk " << chunk->p << "," << chunk->q << std::endl;
                    self->request(chunk_manager, std::chrono::seconds(10), chunk_manager_add::value, chunk);
                    self->request(chunk_manager, std::chrono::seconds(10), chunk_manager_find_neighborhood::value, chunk->p, chunk->q).then(
                            [=](const ChunkNeighbors& neighborhood){
                                self->request(chunk_mesher, std::chrono::seconds(10), chunk, neighborhood).then(
                                        [=](const ChunkMesh& mesh) {
                                            self->request(chunk_manager, std::chrono::seconds(10), chunk_manager_add_mesh::value, p, q, mesh);
                                        }
                                );
                            }
                    );

                }
            );
        }
    }
}



void start_workers(){
    vgk::system.registry().put(vgk::actors::chunk_builder::value, vgk::system.spawn(chunk_builder));
    vgk::system.registry().put(vgk::actors::chunk_mesher::value, vgk::system.spawn(chunk_mesher));
    vgk::system.registry().put(vgk::actors::chunk_manager::value, vgk::system.spawn<ChunkManager>());
    vgk::system.spawn<caf::detached>(build_world);
}