#include "model.h"
#include "util.h"
#include "world.h"
#include "db.h"

Model::Model() {
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

void Model::each_chunk(std::function<void (ChunkPtr chunk)> func) {
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

void Model::draw_loaded_chunks() {
    ChunkPtr chunk = nullptr;
    {
        std::lock_guard<std::mutex> lock_queue(this->queue_mtx);
        if(!this->loading_chunks.empty()){
            std::shared_future<ChunkPtr> &chunk_future = this->loading_chunks.front();
            if(chunk_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready){
                chunk = chunk_future.get();
                this->chunks[std::make_tuple(chunk->p(), chunk->q())] = chunk;
                this->chunk_is_loading[std::make_tuple(chunk->p(), chunk->q())] = false;
                this->loading_chunks.pop();
            }
        }
    }
    if(chunk){
        chunk->generate_buffer();
    }
}


std::future<ChunkPtr> generate_chunk(int p, int q) {
    return std::async(std::launch::async, [=](){
        ChunkPtr chunk = std::make_shared<Chunk>(p, q);
        create_world(chunk, p, q);
        db_load_blocks(chunk, p, q);
        chunk->load();
        return chunk;
    });
}

void Model::request_chunk(int p, int q, bool force) {
    {
        std::lock_guard<std::mutex> lock_queue(this->queue_mtx);
        if(this->chunk_is_loading[std::make_tuple(p,q)]){
            return;
        }
    }


    std::shared_future<ChunkPtr> loading_chunk = generate_chunk(p, q);
    if(force){
        auto chunk = loading_chunk.get();
        this->chunks[std::make_tuple(chunk->p(), chunk->q())] = chunk;
        chunk->generate_buffer();
    } else {
        std::lock_guard<std::mutex> lock_queue(this->queue_mtx);
        this->chunk_is_loading[std::make_tuple(p,q)] = true;
        this->loading_chunks.push(loading_chunk);
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
}