//
// Created by nathan on 1/30/17.
//

#ifndef CRAFT_HEIGHT_MAP_H
#define CRAFT_HEIGHT_MAP_H

#include <cstdio>
#include <stdexcept>
#include <functional>

template<int WIDTH>
class HeightMap {
public:
    char _data[WIDTH][WIDTH] = {{0}};

    HeightMap() {}

    ~HeightMap() {}

    HeightMap(const HeightMap&) = delete;
    HeightMap& operator=(const HeightMap&) = delete;
    HeightMap(HeightMap&&) = delete;
    HeightMap& operator=(HeightMap&&) = delete;

    int set(int x, int z, char w) {
        if(x < 0 || x >= WIDTH || z < 0 || z >= WIDTH){
            printf("Bad Index %d,%d\n", x, z);
            throw new std::invalid_argument("Bad Index");
        }

        int overwrite = 0;
        if(this->_data[x][z] > 0){
            overwrite = 1;
        }
        this->_data[x][z] = w;
        if(w){
            return 1;
        }
        return overwrite;
    }
    char get(int x, int z) {
        if(x < 0 || x >= WIDTH || z < 0 || z >= WIDTH){
            printf("Bad Index %d,%d\n", x, z);
            throw new std::invalid_argument("Bad Index");
        }
        return this->_data[x][z];
    }
    void each(std::function<void (int, int, char)> func) {
        for(int x = 0; x < WIDTH; x++){
            for(int z = 0; z < WIDTH; z++){
                if(this->_data[x][z] != 0){
                    func(x, z, this->_data[x][z]);
                }
            }
        }
    }
    unsigned int size() {
        return WIDTH * WIDTH;
    };
};

#endif //CRAFT_HEIGHT_MAP_H
