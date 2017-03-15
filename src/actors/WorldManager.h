//
// Created by Nathanial Hartman on 3/13/17.
//

#ifndef CRAFT_WORLDMANAGER_H
#define CRAFT_WORLDMANAGER_H

#include <caf/all.hpp>
#include "../chunk/chunk.h"

using ChunkMeshPtr = std::shared_ptr<ChunkMesh>;
using ChunkAndMesh = std::tuple<ChunkPtr, ChunkMeshPtr>;
using ChunkAndMeshMap = std::unordered_map<ChunkPosition, ChunkAndMesh, ChunkPositionHash>;

namespace vgk {
    namespace actors {

        using wm_get_block = caf::atom_constant<caf::atom("wm_gb")>;
        using wm_set_block = caf::atom_constant<caf::atom("wm_sb")>;
        using wm_find_chunk_and_mesh = caf::atom_constant<caf::atom("wm_fcam")>;
        using wm_find_neighbors = caf::atom_constant<caf::atom("wm_fn")>;
        using wm_update = caf::atom_constant<caf::atom("wm_u")>;
        using wm_count_chunks = caf::atom_constant<caf::atom("wm_cc")>;
        using wm_all_chunks = caf::atom_constant<caf::atom("wm_ac")>;

        class WorldManager : public caf::event_based_actor {
        private:
            ChunkAndMeshMap chunks;

        public:
            WorldManager(caf::actor_config& cfg);
            caf::behavior make_behavior() override;

            static ChunkAndMesh find(int p, int q);
            static ChunkNeighbors find_neighbors(int p, int q);
            static void update(int p, int q, const ChunkAndMesh& mesh);

            static char get_block(int x, int y, int z);
            static void set_block(int x, int y, int z, char w);

            static int chunk_count();

            static std::vector<ChunkAndMesh> all_chunks();

            static void load_world();
        };


    }
}

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkAndMesh);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkNeighbors);



#endif //CRAFT_WORLDMANAGER_H
