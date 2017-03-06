//
// Created by Nathanial Hartman on 3/6/17.
//

#include "WorldActor.h"

caf::behavior world(caf::event_based_actor *self) {
    return {
        [=](int p, int q) -> std::string {
            std::stringstream stream;
            stream << "Create Chunk: " << p << "," << q;
            return std::string(stream.str());
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