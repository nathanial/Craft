#include <stdlib.h>
#include "block_map.h"
#include "config.h"
#include <unordered_map>

BlockMap::BlockMap() {
}

BlockMap::~BlockMap() { }


int BlockMap::set(int x, int y, int z, char w) {
    if(x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE){
        printf("Bad Index %d,%d,%d\n", x, y, z);
        throw new std::invalid_argument("Bad Index");
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

char BlockMap::get(int x, int y, int z) {
    if(x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE){
        // printf("Bad Index %d,%d,%d\n", x, y, z);
        //throw new std::invalid_argument("Bad Index");
        return 0;
    }
    return this->_data[x][y][z];
}

unsigned int BlockMap::size() const {
    return CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;
}

void BlockMap::each(std::function<void (int, int, int, char)> func) {
    for(int x = 0; x < CHUNK_SIZE; x++){
        for(int y = 0; y < CHUNK_HEIGHT; y++){
            for(int z = 0; z < CHUNK_SIZE; z++){
                if(this->_data[x][y][z] != 0){
                    func(x, y, z, this->_data[x][y][z]);
                }
            }
        }
    }
}