//
// Created by nathan on 1/21/17.
//

#include <caf/all.hpp>
#include "chunk/chunk.h"

using create_chunk = caf::atom_constant<caf::atom("create")>;

caf::behavior chunk_actor(caf::event_based_actor* self) {
    // return the (initial) actor behavior
    return {
            // a handler for messages containing a single string
            // that replies with a string
            [=](create_chunk, int p, int q) -> Chunk {
                Chunk chunk(p,q);
                return chunk;
            }
    };
}

void build_world(caf::event_based_actor* self, const caf::actor& chunk_creation_actor) {
    for(int p = -10; p < 10; p++){
        for(int q = -10; q < 10; q++){
            self->request(chunk_creation_actor, std::chrono::seconds(10), create_chunk::value, p, q).then(
                [=](const Chunk& chunk) {
                    // ... and print it
                    caf::aout(self) << "Created Chunk " << chunk.p << "," << chunk.q << std::endl;
                }
            );
        }
    }
}

void start_workers(){
    static caf::actor_system_config cfg;
    static caf::actor_system system{cfg};
    system.spawn<caf::detached>(build_world, system.spawn(chunk_actor));
}