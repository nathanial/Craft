#include "model.h"
#include "util.h"

Model::Model(){
}

ChunkPtr Model::get_chunk(int p, int q) {
    if(this->chunks.count(std::make_tuple(p,q)) <= 0){
        return nullptr;
    } else {
        return this->chunks.at(std::make_tuple(p,q));
    }
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

bool Model::is_dirty(int p, int q) {
    return this->dirty_chunks[std::make_tuple(p,q)];
}

void Model::each_chunk(std::function<void (Chunk& chunk)> func) {
    for(const auto & kv : this->chunks){
        auto chunk = kv.second;
        if(chunk == nullptr){
            printf("Missing Chunk %d,%d\n", std::get<0>(kv.first), std::get<1>(kv.first));
            throw "Missing Chunk";
        } else {
            func(*chunk);
        }
    }
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
    this->each_chunk([&](Chunk& chunk){
        int _delete = 1;
        for (int j = 0; j < 3; j++) {
            State *s = states[j];
            int p = chunked(s->x);
            int q = chunked(s->z);
            if (chunk.distance(p, q) < this->delete_radius) {
                _delete = 0;
                break;
            }
        }
        if (_delete) {
            deletion_list.push_back(std::make_tuple(chunk.p(), chunk.q()));
        }
    });
    for(auto & position : deletion_list){
        printf("Delete %d,%d\n", std::get<0>(position), std::get<0>(position));
        this->chunks.erase(position);
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
    this->chunks[std::make_tuple(chunk->p(), chunk->q())] = chunk;
    this->invalidate(chunk->p(),chunk->q());
}

void Model::add_visual_chunk(VisualChunkPtr vchunk) {
    this->visual_chunks[std::make_tuple(vchunk->p, vchunk->q)] = vchunk;
    this->dirty_chunks[std::make_tuple(vchunk->p, vchunk->q)] = false;
}

void Model::invalidate(int p, int q) {
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            this->set_dirty(p + dp, q + dq, true);
        }
    }
}

void Model::set_dirty(int p, int q, bool dirty){
    this->dirty_chunks[std::make_tuple(p,q)] = dirty;
}

bool Model::is_ready_to_draw(int p, int q) {
    return this->visual_chunks.count(std::make_tuple(p,q)) > 0 && this->dirty_chunks[std::make_tuple(p,q)];
}

void Model::each_visual_chunk(std::function<void (VisualChunk& vchunk)> func) {
    for(const auto & kv : this->visual_chunks){
        auto chunk = kv.second;
        if(!chunk){
            printf("Missing Chunk %d,%d\n", std::get<0>(kv.first), std::get<1>(kv.first));
            throw "Missing Chunk";
        } else {
            func(*chunk);
        }
    }
}

void Model::generate_chunk_buffer(int a, int b) {
    if(this->visual_chunks.count(std::make_tuple(a,b)) <= 0){
        return;
    }
    auto vchunk = this->visual_chunks.at(std::make_tuple(a,b));
    this->visual_chunks[std::make_tuple(a,b)] = generate_buffer(*vchunk);
    this->dirty_chunks[std::make_tuple(a,b)] = false;

}