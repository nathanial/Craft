//
// Created by Nathanial Hartman on 3/13/17.
//

#ifndef CRAFT_WORLDMANAGER_H
#define CRAFT_WORLDMANAGER_H

#include <caf/all.hpp>
#include "../chunk/chunk.h"

using ChunkMeshPtr = std::shared_ptr<ChunkMesh>;

class ChunkAndMesh {
public:
    ChunkPtr chunk;
    ChunkMeshPtr mesh;
    ChunkAndMesh() : chunk(nullptr), mesh(nullptr) {}
    ChunkAndMesh(const ChunkPtr chunk, const ChunkMeshPtr mesh) : chunk(chunk), mesh(mesh) {}
};

using ChunksAndMeshes = std::vector<ChunkAndMesh>;
using ChunkAndMeshMap = std::unordered_map<ChunkPosition, ChunkAndMesh, ChunkPositionHash>;

namespace vgk {
    namespace actors {

        using wm_get_block = caf::atom_constant<caf::atom("wmgb")>;
        using wm_set_block = caf::atom_constant<caf::atom("wmsb")>;
        using wm_find_chunk_and_mesh = caf::atom_constant<caf::atom("wmfcam")>;
        using wm_find_neighbors = caf::atom_constant<caf::atom("wmfn")>;
        using wm_update = caf::atom_constant<caf::atom("wmu")>;
        using wm_count_chunks = caf::atom_constant<caf::atom("wmcc")>;
        using wm_all_chunks = caf::atom_constant<caf::atom("wmac")>;
        using wm_load_world = caf::atom_constant<caf::atom("wmlw")>;

        class World : public caf::event_based_actor {
        private:
            ChunkAndMeshMap chunks;

            char internal_get_block(int x, int y, int z);
            void internal_set_block(int x, int y, int z, char w);
            ChunkAndMesh internal_find_chunk_and_mesh(int p, int q);
            ChunkNeighbors internal_find_neighbors(int p, int q);
            void internal_update(int p, int q, const ChunkAndMesh& chunk_and_mesh);
            int internal_count_chunks();
            ChunksAndMeshes internal_all_chunks();
            void internal_load_world();

        public:
            World(caf::actor_config& cfg);
            caf::behavior make_behavior() override;

            static ChunkAndMesh find(int p, int q);
            static ChunkNeighbors find_neighbors(int p, int q);
            static void update(int p, int q, const ChunkAndMesh& mesh);

            static char get_block(int x, int y, int z);
            static void set_block(int x, int y, int z, char w);

            static int chunk_count();

            static ChunksAndMeshes all_chunks();

            static void load_world();
        };


    }
}

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkPtr);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkMeshPtr);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkAndMesh);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkNeighbors);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunksAndMeshes);



#endif //CRAFT_WORLDMANAGER_H
