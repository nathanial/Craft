#include <stdlib.h>
#include <string.h>
#include "map.h"
#include <functional>

int hash_int(int key) {
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key;
}

int hash(int x, int y, int z) {
    x = hash_int(x);
    y = hash_int(y);
    z = hash_int(z);
    return x ^ y ^ z;
}

Map::Map(int dx, int dy, int dz, unsigned int mask) {
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
    this->mask = mask;
    this->_size = 0;
    this->data = (MapEntry *)calloc(this->mask + 1, sizeof(MapEntry));
}

Map::~Map() {
    if(this->data != nullptr){
        free(this->data);
    }
}

Map* Map::clone() {
    Map *m = new Map(this->dx, this->dy, this->dz, this->mask);
    m->_size = this->_size;
    memcpy(m->data, this->data, (m->mask + 1) * sizeof(MapEntry));
    return m;
}

int Map::set(int x, int y, int z, int w) {
    unsigned int index = hash(x, y, z) & this->mask;
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    MapEntry *entry = this->data + index;
    int overwrite = 0;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z) {
            overwrite = 1;
            break;
        }
        index = (index + 1) & this->mask;
        entry = this->data + index;
    }
    if (overwrite) {
        if (entry->e.w != w) {
            entry->e.w = w;
            return 1;
        }
    }
    else if (w) {
        entry->e.x = x;
        entry->e.y = y;
        entry->e.z = z;
        entry->e.w = w;
        this->_size++;
        if (this->_size * 2 > this->mask) {
            this->grow();
        }
        return 1;
    }
    return 0;
}

int Map::get(int x, int y, int z) {
    unsigned int index = hash(x, y, z) & this->mask;
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    if (x < 0 || x > 255) return 0;
    if (y < 0 || y > 255) return 0;
    if (z < 0 || z > 255) return 0;
    MapEntry *entry = this->data + index;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z) {
            return entry->e.w;
        }
        index = (index + 1) & this->mask;
        entry = this->data + index;
    }
    return 0;
}

void Map::grow() {
    Map new_map(this->dx, this->dy, this->dz, (this->mask << 1) | 1);
    this->each([&](int ex, int ey, int ez, int ew) {
        new_map.set(ex, ey, ez, ew);
    });
    free(this->data);
    this->mask = new_map.mask;
    this->_size = new_map._size;
    this->data = new_map.data;
    new_map.data = nullptr;
}

unsigned int Map::size() const {
    return this->_size;
}

void Map::each(std::function<void (int, int, int, int)> func) {
    for (unsigned int i = 0; i <= this->mask; i++) {
        MapEntry *entry = this->data + i;
        if (EMPTY_ENTRY(entry)) {
            continue;
        }
        int ex = entry->e.x + this->dx;
        int ey = entry->e.y + this->dy;
        int ez = entry->e.z + this->dz;
        int ew = entry->e.w;

        func(ex,ey,ez,ew);
    }
}