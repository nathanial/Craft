#include "model.h"
#include "util.h"
#include "chunk/ChunkMesh.h"
#include "chunk/TransientChunkMesh.h"

Model::Model(){
}

ChunkPtr Model::get_chunk(int p, int q) {
    if(this->chunks.count(std::make_tuple(p,q)) <= 0){
        return nullptr;
    } else {
        return this->chunks.at(std::make_tuple(p,q));
    }
}

std::shared_ptr<ChunkMesh> Model::get_mesh(int p, int q) {
    if(this->meshes.count(std::make_tuple(p,q)) <= 0){
        return nullptr;
    } else {
        return this->meshes.at(std::make_tuple(p,q));
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

std::shared_ptr<ChunkMesh> Model::find_mesh(int p, int q) {
    auto mesh = this->get_mesh(p,q);
    if(mesh == nullptr){
        return nullptr;
    }
    return mesh;
}


void Model::update_chunk(int p, int q, std::function<void (TransientChunk&) > func){
    auto chunk = this->find_chunk(p,q);
    auto transient = chunk->transient();
    func(*transient);
    this->replace_chunk(std::make_shared<Chunk>(transient->immutable()));
}

void Model::update_mesh(int p, int q, std::function<void (TransientChunkMesh&) > func){
    auto mesh = this->find_mesh(p,q);
    if(mesh){
        auto transient = mesh->transient();
        func(*transient);
        this->replace_mesh(p,q, std::make_shared<ChunkMesh>(transient->immutable()));
    } else {
        TransientChunkMesh transient;
        func(transient);
        this->replace_mesh(p,q, std::make_shared<ChunkMesh>(transient.immutable()));
    }
}

void Model::replace_mesh(int p, int q, std::shared_ptr<ChunkMesh> mesh){
    this->meshes[std::make_tuple(p, q)] = mesh;
}

void Model::replace_chunk(std::shared_ptr<Chunk> chunk){
    this->chunks[std::make_tuple(chunk->p, chunk->q)] = chunk;
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
    if(y < 0){
        return 0;
    }
    return chunk->get_block(x,y,z);
}

void Model::delete_chunks(){
    State *s1 = &this->player.state;
    State *s2 = &this->player.state;
    State *s3 = &this->player.state;
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
            deletion_list.push_back(std::make_tuple(chunk.p, chunk.q));
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