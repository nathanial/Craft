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
}


std::future<ChunkPtr> generate_chunk(int p, int q, NeighborEdgesPtr edges) {
    std::promise<ChunkPtr> chunk_promise;
    ChunkPtr chunk = std::make_shared<Chunk>(p, q);
    create_world(chunk, p, q);
    db_load_blocks(chunk, p, q);
    chunk->load(edges);
    chunk_promise.set_value(chunk);
    return chunk_promise.get_future();
}

void Model::request_chunk(int p, int q, bool force) {
    NeighborEdgesPtr edges;
    {
        if(this->chunk_is_loading[std::make_tuple(p,q)]){
            return;
        }
        if(this->find_chunk(p,q)) {
            return;
        }
        edges = this->find_edges(p,q);
    }


    std::shared_future<ChunkPtr> loading_chunk = generate_chunk(p, q, edges);
    auto chunk = loading_chunk.get();
    this->chunks[std::make_tuple(chunk->p(), chunk->q())] = chunk;
    chunk->generate_buffer();
    chunk->set_dirty(false);
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

NorthSouthEdgeMap south_edge_blocks(std::function<char (int,int,int)> func) {
    std::shared_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, 1>> blocks(new BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, 1>);
    for(int x = 0; x < CHUNK_SIZE; x++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            blocks->set(x,y,0, func(x,y,CHUNK_SIZE-1));
        }
    }
    return blocks;
};

NorthSouthEdgeMap north_edge_blocks(std::function<char (int,int,int)> func) {
    std::shared_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, 1>> blocks(new BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, 1>);
    for(int x = 0; x < CHUNK_SIZE; x++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            blocks->set(x,y,0, func(x,y,0));
        }
    }
    return blocks;
};

EastWestEdgeMap west_edge_blocks(std::function<char (int,int,int)> func) {
    std::shared_ptr<BlockMap<1, CHUNK_HEIGHT, CHUNK_SIZE>> blocks(new BlockMap<1, CHUNK_HEIGHT, CHUNK_SIZE>);
    for(int z = 0; z < CHUNK_SIZE; z++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            blocks->set(0,y,z, func(0,y,z));
        }
    }
    return blocks;
};

EastWestEdgeMap east_edge_blocks(std::function<char (int,int,int)> func) {
    std::shared_ptr<BlockMap<1, CHUNK_HEIGHT, CHUNK_SIZE>> blocks(new BlockMap<1, CHUNK_HEIGHT, CHUNK_SIZE>);
    for(int z = 0; z < CHUNK_SIZE; z++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            auto block = func(CHUNK_SIZE-1,y,z);
            blocks->set(0,y,z, block);
        }
    }
    return blocks;
};

NeighborEdgesPtr Model::find_edges(int p, int q){
    auto north_chunk = this->find_chunk(p, q-1);
    auto south_chunk = this->find_chunk(p, q+1);
    auto west_chunk = this->find_chunk(p-1, q);
    auto east_chunk = this->find_chunk(p+1,q);

    NeighborEdgesPtr edges = std::make_shared<NeighborEdges>();
    if(north_chunk){
        edges->north_edge_blocks = south_edge_blocks([&](int x, int y, int z){
            return north_chunk->blocks->get(x,y,z);
        });
        edges->north_edge_lights = south_edge_blocks([&](int x, int y, int z){
            // exclude south light levels, because those are from the central chunk
            return (
                north_chunk->native_light_levels->get(x,y,z) +
                north_chunk->north_light_levels->get(x,y,z) +
                north_chunk->east_light_levels->get(x,y,z) +
                north_chunk->west_light_levels->get(x,y,z)
            );
        });
    }
    if(south_chunk){
        edges->south_edge_blocks = north_edge_blocks([&](int x, int y, int z){
            return south_chunk->blocks->get(x,y,z);
        });
        edges->south_edge_lights = north_edge_blocks([&](int x, int y, int z) {
            // exclude north light levels, because those are from the central chunk
            return (
                south_chunk->native_light_levels->get(x,y,z) +
                south_chunk->south_light_levels->get(x,y,z) +
                south_chunk->west_light_levels->get(x,y,z) +
                south_chunk->east_light_levels->get(x,y,z)
            );
        });
    }
    if(west_chunk){
        edges->west_edge_blocks = east_edge_blocks([&](int x, int y, int z){
            return west_chunk->blocks->get(x,y,z);
        });
        edges->west_edge_lights = east_edge_blocks([&](int x, int y, int z) {
            // exclude east light levels, because those are from the central chunk
            return (
                west_chunk->native_light_levels->get(x,y,z) +
                west_chunk->north_light_levels->get(x,y,z) +
                west_chunk->south_light_levels->get(x,y,z) +
                west_chunk->west_light_levels->get(x,y,z)
            );
        });
    }
    if(east_chunk){
        edges->east_edge_blocks = west_edge_blocks([&](int x, int y, int z){
            return east_chunk->blocks->get(x,y,z);
        });
        edges->east_edge_lights = west_edge_blocks([&](int x, int y, int z){
            // exclude west light levels, because those are from the central chunk
            return (
              east_chunk->native_light_levels->get(x,y,z) +
                    east_chunk->north_light_levels->get(x,y,z) +
                    east_chunk->south_light_levels->get(x,y,z) +
                    east_chunk->east_light_levels->get(x,y,z)
            );
        });
    }
    return edges;
}

void Model::reload_chunk(int p, int q){
    auto chunk = this->find_chunk(p,q);
    if(chunk){
        this->chunk_is_loading[std::make_tuple(chunk->p(), chunk->q())] = true;
        auto edges = this->find_edges(chunk->p(), chunk->q());
        chunk->load(edges);
        chunk->set_dirty(true);
    }
}