//
// Created by nathan on 2/8/17.
//

#include "chunk_map.h"
#include <mutex>
#include <shared_mutex>

ChunkPtr ChunkMap::get_chunk(const ChunkPosition &position) const {
    std::shared_lock<std::shared_mutex> lock(this->mtx);
    if(this->chunks.count(position) <= 0){
        return nullptr;
    } else {
        return this->chunks.at(position);
    }
}

void ChunkMap::set_chunk(const ChunkPosition &position, ChunkPtr chunk) {
    std::unique_lock<std::shared_mutex> lock(this->mtx);
    this->chunks[position] = chunk;
}

void ChunkMap::clear(){
    std::unique_lock<std::shared_mutex> lock(this->mtx);
    this->chunks.clear();
}
void ChunkMap::each_chunk(std::function<void (ChunkPtr chunk)> func) {
    std::shared_lock<std::shared_mutex> lock(this->mtx);
    for(const auto & kv : this->chunks){
        auto chunk = kv.second;
        if(chunk == nullptr){
            printf("Missing Chunk %d,%d\n", std::get<0>(kv.first), std::get<1>(kv.first));
            throw "Missing Chunk";
        } else {
            func(chunk);
        }
    }
}

void ChunkMap::remove(const ChunkPosition &position) {
    std::unique_lock<std::shared_mutex> lock(this->mtx);
    this->chunks.erase(position);
}

unsigned long ChunkMap::size() const {
    std::shared_lock<std::shared_mutex> lock(this->mtx);
    return this->chunks.size();
}