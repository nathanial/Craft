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
    this->size = 0;
    this->data = (MapEntry *)calloc(this->mask + 1, sizeof(MapEntry));
}

Map::~Map() {
    if(this->data != nullptr){
        free(this->data);
    }
}

void map_copy(Map *dst, Map *src) {
    dst->dx = src->dx;
    dst->dy = src->dy;
    dst->dz = src->dz;
    dst->mask = src->mask;
    dst->size = src->size;
    dst->data = (MapEntry *)calloc(dst->mask + 1, sizeof(MapEntry));
    memcpy(dst->data, src->data, (dst->mask + 1) * sizeof(MapEntry));
}

int map_set(Map *map, int x, int y, int z, int w) {
    unsigned int index = hash(x, y, z) & map->mask;
    x -= map->dx;
    y -= map->dy;
    z -= map->dz;
    MapEntry *entry = map->data + index;
    int overwrite = 0;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z) {
            overwrite = 1;
            break;
        }
        index = (index + 1) & map->mask;
        entry = map->data + index;
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
        map->size++;
        if (map->size * 2 > map->mask) {
            map_grow(map);
        }
        return 1;
    }
    return 0;
}

int map_get(const Map *map, int x, int y, int z) {
    unsigned int index = hash(x, y, z) & map->mask;
    x -= map->dx;
    y -= map->dy;
    z -= map->dz;
    if (x < 0 || x > 255) return 0;
    if (y < 0 || y > 255) return 0;
    if (z < 0 || z > 255) return 0;
    MapEntry *entry = map->data + index;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z) {
            return entry->e.w;
        }
        index = (index + 1) & map->mask;
        entry = map->data + index;
    }
    return 0;
}

void map_grow(Map *map) {
    Map new_map(map->dx,map->dy,map->dz, (map->mask << 1) | 1);
    map_for_each(map, [&](int ex, int ey, int ez, int ew) {
        map_set(&new_map, ex, ey, ez, ew);
    });
    free(map->data);
    map->mask = new_map.mask;
    map->size = new_map.size;
    map->data = new_map.data;
    new_map.data = nullptr;
}


void map_for_each(Map *map, std::function<void (int, int, int, int)> func) {
    for (unsigned int i = 0; i <= map->mask; i++) {
        MapEntry *entry = map->data + i;
        if (EMPTY_ENTRY(entry)) {
            continue;
        }
        int ex = entry->e.x + map->dx;
        int ey = entry->e.y + map->dy;
        int ez = entry->e.z + map->dz;
        int ew = entry->e.w;

        func(ex,ey,ez,ew);
    }
}