//
// Created by nathan on 1/21/17.
//

#ifndef CRAFT_WORKER_H
#define CRAFT_WORKER_H

#include <caf/all.hpp>
#include <future>

namespace vgk {
    namespace actors {
        extern caf::actor_system_config cfg;
        extern caf::actor_system * system;

        using chunk_builder_id = caf::atom_constant<caf::atom("cbaid")>;
        using world_manager_id = caf::atom_constant<caf::atom("wmid")>;

        using create_chunk = caf::atom_constant<caf::atom("create")>;

        using load_nearby_chunks = caf::atom_constant<caf::atom("wblnc")>;

        void start();
    }
}


#endif //CRAFT_WORKER_H
