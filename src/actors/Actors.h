//
// Created by nathan on 1/21/17.
//

#ifndef CRAFT_WORKER_H
#define CRAFT_WORKER_H

#include <caf/all.hpp>
#include <future>

namespace vgk {
    namespace actors {
        static caf::actor_system_config cfg;
        static caf::actor_system system{cfg};

        using chunk_builder_id = caf::atom_constant<caf::atom("cbaid")>;
        using chunk_mesher_id = caf::atom_constant<caf::atom("cmaid")>;
        using world_manager_id = caf::atom_constant<caf::atom("wmid")>;

        using create_chunk = caf::atom_constant<caf::atom("create")>;
        using wm_get_block = caf::atom_constant<caf::atom("wm_gb")>;

        void start();

        class WorldManager : public caf::event_based_actor {
        public:
            WorldManager(caf::actor_config& cfg);

            caf::behavior make_behavior() override;
        };
    }
}


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(std::shared_future<char>);

#endif //CRAFT_WORKER_H
