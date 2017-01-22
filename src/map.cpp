#include <stdlib.h>
#include "map.h"
#include "config.h"
#include <unordered_map>

Map::Map(int dx, int dy, int dz) {
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
}

Map::~Map() { }

Map* Map::clone() {
    Map *m = new Map(this->dx, this->dy, this->dz);
    m->_data = this->_data;
    return m;
}

int Map::set(int x, int y, int z, char w) {

    x -= this->dx;
    y -= this->dy;
    z -= this->dz;

    if(x < 0 || x > CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z > CHUNK_SIZE ){
        //printf("Bad Index %d,%d,%d\n", x, y, z);
        return 0;
    }

    MapEntry entry { x, y, z };
    int overwrite = 0;
    if(this->_data.count(entry) > 0){
        overwrite = 1;
    }
    this->_data[entry] = w;
    if(w){
        return 1;
    }
    return overwrite;
}

char Map::get(int x, int y, int z) {
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    MapEntry entry = { x, y, z };
    if(x < 0 || x > CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z > CHUNK_SIZE){
        // printf("Bad Index %d,%d,%d\n", x, y, z);
        return 0;
    }
    return this->_data[entry];
}

unsigned int Map::size() const {
    return this->_data.size();
}

void Map::each(std::function<void (int, int, int, char)> func) {
    for (const auto& kv : this->_data) {
        func(kv.first.x + this->dx, kv.first.y + this->dy, kv.first.z + this->dz, kv.second);
    }
}