#include <stdlib.h>
#include "map.h"
#include <unordered_map>

Map::Map(int dx, int dy, int dz) {
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
}

Map::~Map() {
}

static int minX = 0;
static int maxX = 0;
static int minY = 0;
static int maxY = 0;
static int minZ = 0;
static int maxZ = 0;

Map* Map::clone() {
    Map *m = new Map(this->dx, this->dy, this->dz);
    m->_data = this->_data;
    return m;
}

int Map::set(int x, int y, int z, char w) {
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    MapEntry entry;
    entry.x = x;
    entry.y = y;
    entry.z = z;
    if(x < minX){
        minX = x;
    }
    if(x > maxX){
        maxX = x;
    }
    if(y < minY){
        minY = y;
    }
    if(y > maxY){
        maxY = y;
    }
    if(z < minZ){
        minZ = z;
    }
    if(z > maxZ) {
        maxZ = z;
    }
    // printf("%d..%d,%d..%d,%d..%d\n", minX, maxX, minY, maxY, minZ, maxZ);
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
    MapEntry entry;
    entry.x = x;
    entry.y = y;
    entry.z = z;
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