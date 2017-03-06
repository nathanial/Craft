//
// Created by Nathanial Hartman on 3/6/17.
//

#ifndef CRAFT_WORLDACTOR_H
#define CRAFT_WORLDACTOR_H

#include "caf/all.hpp"
#include <iostream>
#include <sstream>

struct CreateChunkRequest {
    int p, q;
};

caf::behavior world(caf::event_based_actor *self);
void chunk_creator(caf::event_based_actor* self, const caf::actor& buddy);


#endif //CRAFT_WORLDACTOR_H
