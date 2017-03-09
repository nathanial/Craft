//
// Created by nathan on 1/21/17.
//

#ifndef CRAFT_WORKER_H
#define CRAFT_WORKER_H

#include <caf/all.hpp>

namespace vgk {
    namespace actors {
        static caf::actor_system_config cfg;
        static caf::actor_system system{cfg};

        using chunk_builder_id = caf::atom_constant<caf::atom("cbaid")>;
        using chunk_mesher_id = caf::atom_constant<caf::atom("cmaid")>;

        void start();
    }
}



#endif //CRAFT_WORKER_H
