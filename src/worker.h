//
// Created by nathan on 1/21/17.
//

#ifndef CRAFT_WORKER_H
#define CRAFT_WORKER_H

#include <caf/all.hpp>

namespace vgk {
    caf::actor_system_config cfg;
    caf::actor_system system {cfg};
}

void start_workers();

#endif //CRAFT_WORKER_H
