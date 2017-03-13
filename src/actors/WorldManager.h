//
// Created by Nathanial Hartman on 3/13/17.
//

#ifndef CRAFT_WORLDMANAGER_H
#define CRAFT_WORLDMANAGER_H

#include <caf/all.hpp>

namespace vgk {
    namespace actors {

        using wm_get_block = caf::atom_constant<caf::atom("wm_gb")>;
        using wm_set_block = caf::atom_constant<caf::atom("wm_sb")>;

        class WorldManager : public caf::event_based_actor {
        public:
            WorldManager(caf::actor_config& cfg);

            caf::behavior make_behavior() override;
        };

    }
}


#endif //CRAFT_WORLDMANAGER_H
