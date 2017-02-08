#include "model.h"
#include "util.h"

Model::Model(){
    for(int i = 0; i < WORKERS; i++){
        this->workers.push_back(std::make_shared<Worker>());
    }
}

ChunkPtr Model::get_chunk(int p, int q) {
    return this->chunks.get_chunk(std::make_tuple(p,q));
}

void Model::clear_chunks(){
    this->chunks.clear();
}

ChunkPtr Model::find_chunk(int p, int q) {
    auto chunk = this->get_chunk(p,q);
    if(chunk == nullptr){
        return nullptr;
    }
    return chunk;
}



char Model::get_block(int x, int y, int z) {
    int p = chunked(x);
    int q = chunked(z);
    auto chunk = this->find_chunk(p, q);
    if(!chunk){
        return 0;
    }
    return chunk->get_block(x,y,z);
}

void Model::delete_chunks(){
    State *s1 = &this->players->state;
    State *s2 = &(this->players + this->observe1)->state;
    State *s3 = &(this->players + this->observe2)->state;
    State *states[3] = {s1, s2, s3};
    std::vector<ChunkPosition> deletion_list;
    this->each_chunk([&](ChunkPtr chunk){
        int _delete = 1;
        for (int j = 0; j < 3; j++) {
            State *s = states[j];
            int p = chunked(s->x);
            int q = chunked(s->z);
            if (chunk->distance(p, q) < this->delete_radius) {
                _delete = 0;
                break;
            }
        }
        if (_delete) {
            deletion_list.push_back(std::make_tuple(chunk->p(), chunk->q()));
        }
    });
    for(auto & position : deletion_list){
        printf("Delete %d,%d\n", std::get<0>(position), std::get<0>(position));
        this->chunks.remove(position);
    }
}

void Model::delete_all_chunks() {
    this->chunks.clear();
}

int Model::chunk_count() const {
    return static_cast<int>(this->chunks.size());
}

void Model::add_chunk(ChunkPtr chunk) {
    if(chunk == nullptr){
        throw "Couldn't Make Shared Ptr";
    }
    this->chunks.set_chunk(std::make_tuple(chunk->p(), chunk->q()), chunk);
}

void Model::each_chunk(std::function<void (ChunkPtr chunk)> func) {
    this->chunks.each_chunk(func);
}