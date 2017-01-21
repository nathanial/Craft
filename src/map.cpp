#include <stdlib.h>
#include <string.h>
#include "map.h"
#include <functional>

Map::Map(int dx, int dy, int dz) {
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
}

Map::~Map() {
}

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
    entry.e.x = x;
    entry.e.y = y;
    entry.e.z = z;
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
    entry.e.x = x;
    entry.e.y = y;
    entry.e.z = z;
    return this->_data[entry];
}

unsigned int Map::size() const {
    return this->_data.size();
}

void Map::each(std::function<void (int, int, int, char)> func) {
    for (const auto& kv : this->_data) {
        func(kv.first.e.x + this->dx, kv.first.e.y + this->dy, kv.first.e.z + this->dz, kv.second);
    }
}