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
    std::copy(&this->_data[0][0][0], &this->_data[0][0][0] + (CHUNK_SIZE)*(CHUNK_SIZE)*CHUNK_HEIGHT,  &m->_data[0][0][0]);
    return m;
}

int Map::set(int x, int y, int z, char w) {
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;

    if(x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE){
        printf("Bad Index %d,%d,%d\n", x, y, z);
//        throw new std::invalid_argument("oops");
        return 0;
    }

    int overwrite = 0;
    if(this->_data[x][y][z] > 0){
        overwrite = 1;
    }
    this->_data[x][y][z] = w;
    if(w){
        return 1;
    }
    return overwrite;
}

char Map::get(int x, int y, int z) {
    x -= this->dx;
    y -= this->dy;
    z -= this->dz;
    if(x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE){
        // printf("Bad Index %d,%d,%d\n", x, y, z);
        return 0;
    }
    return this->_data[x][y][z];
}

unsigned int Map::size() const {
    return CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;
}

void Map::each(std::function<void (int, int, int, char)> func) {
    for(int x = 0; x < CHUNK_SIZE; x++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            for(int z = 0; z < CHUNK_SIZE; z++){
                if(this->_data[x][y][z] != 0){
                    func(x + this->dx, y + this->dy, z + this->dz, this->_data[x][y][z]);
                }
            }
        }
    }
}