//
// Created by Nathanial Hartman on 3/9/17.
//

#include "ChunkManager.h"

static ChunkNeighbors find_neighbors(ChunkMap& allChunks, int p, int q){
    ChunkNeighbors neighbors;
    for(int dp = -1; dp <= 1; dp++){
        for(int dq = -1; dq <= 1; dq++){
            auto position = std::make_tuple(dp + p, dq + q);
            if(allChunks.count(position) > 0){
                neighbors[position] = allChunks.at(position);
            } else {
                neighbors[position] = nullptr;
            }
        }
    }
    return neighbors;
}

ChunkManager::ChunkManager(caf::actor_config& cfg) : caf::event_based_actor(cfg) {
}

caf::behavior ChunkManager::make_behavior() {
    return {
            [&](chunk_manager_add, ChunkPtr val){
                this->_chunks[std::make_tuple(val->p, val->q)] = val;
            },
            [&](chunk_manager_remove, int p, int q) {
                this->_chunks.erase(std::make_tuple(p,q));
            },
            [&](chunk_manager_find_neighborhood, int p, int q){
                return find_neighbors(this->_chunks, p, q);
            },
            [&](chunk_manager_add_mesh, int p, int q, const ChunkMesh &mesh){
                this->_meshes[std::make_tuple(p,q)] = std::make_shared<ChunkMesh>(mesh);
            }
    };
}