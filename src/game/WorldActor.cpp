//
// Created by Nathanial Hartman on 3/6/17.
//

#include "WorldActor.h"
#include "../chunk/chunk.h"

using create_chunk = caf::atom_constant<caf::atom("create")>;
using delete_chunk = caf::atom_constant<caf::atom("delete")>;

caf::behavior world(caf::event_based_actor *self) {
    return {
//        [](create_chunk, int p, int q) -> Chunk {
//            Chunk chunk(p,q);
//            return chunk;
//        },
        [](delete_chunk, int p, int q) -> bool {
            return true;
        }
    };
}

void chunk_creator(caf::event_based_actor* self, const caf::actor& buddy) {
    // send "Hello World!" to our buddy ...
    self->request(buddy, std::chrono::seconds(10), 10, 11).then(
            // ... wait up to 10s for a response ...
            [=](const std::string& what) {
                // ... and print it
                caf::aout(self) << "Chunk Creator Got Response: " << what << std::endl;
            }
    );
}