#include "model.h"
#include "util.h"
#include "item.h"

Model::Model(){
    for(int i = 0; i < WORKERS; i++){
        this->workers.push_back(std::make_shared<Worker>());
    }
}

ChunkPtr Model::get_chunk(int p, int q) {
    std::shared_lock<std::shared_mutex> lock(this->chunk_mtx);
    if(this->chunks.count(std::make_tuple(p,q)) <= 0){
        return nullptr;
    } else {
        return this->chunks.at(std::make_tuple(p,q));
    }
}

void Model::clear_chunks(){
    std::unique_lock<std::shared_mutex> lock(this->chunk_mtx);
    this->chunks.clear();
}

ChunkPtr Model::find_chunk(int p, int q) {
    std::shared_lock<std::shared_mutex> lock(this->chunk_mtx);
    auto chunk = this->get_chunk(p,q);
    if(chunk == nullptr){
        return nullptr;
    }
    return chunk;
}

ChunkMeshPtr Model::find_mesh(int p, int q) {
    std::shared_lock<std::shared_mutex> lock(this->mesh_mtx);
    if(this->meshes.count(std::make_tuple(p,q)) <= 0) {
        return nullptr;
    } else {
        return this->meshes.at(std::make_tuple(p, q));
    }
}

void Model::each_chunk(std::function<void (ChunkPtr chunk)> func) {
    std::shared_lock<std::shared_mutex> lock(this->chunk_mtx);
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

void Model::each_mesh(std::function<void (ChunkMeshPtr mesh)> func) {
    std::shared_lock<std::shared_mutex> lock(this->mesh_mtx);
    for(const auto & kv : this->meshes) {
        auto mesh = kv.second;
        if(mesh == nullptr){
            printf("Missing Mesh %d,%d\n", std::get<0>(kv.first), std::get<1>(kv.first));
            throw "Missing Mesh";
        } else {
            func(mesh);
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
    std::unique_lock<std::shared_mutex> lock(this->chunk_mtx);
    for(auto & position : deletion_list){
        printf("Delete %d,%d\n", std::get<0>(position), std::get<0>(position));
        this->chunks.erase(position);
    }
}

void Model::delete_all_chunks() {
    std::unique_lock<std::shared_mutex> lock(this->chunk_mtx);
    this->chunks.clear();
}

int Model::chunk_count() const {
    std::shared_lock<std::shared_mutex> lock(this->chunk_mtx);
    return static_cast<int>(this->chunks.size());
}

void Model::add_chunk(ChunkPtr chunk) {
    {
        std::unique_lock<std::shared_mutex> lock(this->chunk_mtx);
        if(chunk == nullptr){
            throw "Couldn't Make Shared Ptr";
        }
        this->chunks[std::make_tuple(chunk->p(), chunk->q())] = chunk;
    }
    this->set_dirty_flag(chunk->p(), chunk->q());
}

void Model::add_mesh(ChunkMeshPtr mesh) {
    {
        std::unique_lock<std::shared_mutex> lock(this->mesh_mtx);
        this->meshes[std::make_tuple(mesh->p, mesh->q)] = mesh;
    }
}

void Model::set_dirty_flag(int p, int q) {
    auto mesh = this->find_mesh(p, q);
    if(mesh){
        mesh->set_dirty(true);
    }
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            auto other = this->find_mesh(p + dp, q + dq);
            if (other) {
                other->set_dirty(true);
            }
        }
    }
}

int Model::highest_block(float x, float z){
    int result = -1;
    int nx = roundf(x);
    int nz = roundf(z);
    int p = chunked(x);
    int q = chunked(z);
    printf("Highest Block %f %f\n", x, z);
    auto chunk = this->find_chunk(p, q);
    if (chunk) {
        chunk->foreach_block([&](int ex, int ey, int ez, int ew){
            if (is_obstacle(ew) && ex == nx && ez == nz) {
                result = MAX(result, ey);
            }
        });
    }
    return result;
}
