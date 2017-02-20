#include "model.h"
#include "util.h"
#include "workers/tasks/generate_chunk_task.h"

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
    State *s = &this->player.state;
    std::vector<ChunkPosition> deletion_list;
    this->each_chunk([&](Chunk& chunk){
        int _delete = 1;
        int p = chunked(s->x);
        int q = chunked(s->z);
        if (chunk.distance(p, q) < this->delete_radius) {
            _delete = 0;
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
    printf("Add Visual Chunk %d,%d\n", vchunk->p, vchunk->q);
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

bool Model::has_visual_chunk(int a, int b){
    return this->visual_chunks.count(std::make_tuple(a,b)) > 0;
}

VisualChunkPtr Model::find_visual_chunk(int a, int b){
    return this->visual_chunks.at(std::make_tuple(a,b));
}

void Model::load_async_chunks(){
    for(int i = 0; i < 100 && i < this->vchunk_futures.size(); i++){
        auto &vfuture = this->vchunk_futures.front();
        auto status = vfuture.wait_for(std::chrono::seconds(0));
        if(status == std::future_status::ready){
            auto vchunk = vfuture.get();
            vchunk = generate_buffer(*vchunk);
            this->add_visual_chunk(vchunk);
            this->vchunk_futures.pop_front();
        } else {
            this->vchunk_futures.push_back(vfuture);
            this->vchunk_futures.pop_front();
        }
    }
}