#ifndef _map_h_
#define _map_h_

#include <functional>
#include <cmath>
#include <unordered_map>
#include "config.h"

template<unsigned int WIDTH, unsigned int HEIGHT>
class BlockMap {
public:
    short _data[WIDTH][HEIGHT][WIDTH] = {{{0}}};

    BlockMap() {}

    ~BlockMap() {}

    BlockMap(const BlockMap&) = delete;
    BlockMap& operator=(const BlockMap&) = delete;
    BlockMap(BlockMap&&) = delete;
    BlockMap& operator=(BlockMap&&) = delete;

    void set(int x, int y, int z, short w) {
        if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || z < 0 || z >= WIDTH){
            printf("Set Bad Index %d,%d,%d\n", x, y, z);
            throw new std::invalid_argument("Bad Index");
        }
        this->_data[x][y][z] = w;
    }
    short get(int x, int y, int z) const {
        if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT|| z < 0 || z >= WIDTH){
            printf("Get Bad Index %d,%d,%d\n", x, y, z);
            throw new std::invalid_argument("Bad Index");
        }
        return this->_data[x][y][z];
    }
    short get_or_default(int x, int y, int z, short _default = 0) const {
        if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT|| z < 0 || z >= WIDTH){
            return _default;
        }
        return this->_data[x][y][z];
    }
    void each(std::function<void (int, int, int, char)> func) {
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                for(int z = 0; z < WIDTH; z++){
                    func(x, y, z, this->_data[x][y][z]);
                }
            }
        }
    }
    void each(std::function<void (int, int, int, char)> func) const {
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                for(int z = 0; z < WIDTH; z++){
                    func(x, y, z, this->_data[x][y][z]);
                }
            }
        }
    }
    unsigned int size() const {
        return WIDTH * WIDTH * HEIGHT;
    };

    std::unique_ptr<BlockMap<WIDTH, HEIGHT>> copy() const {
        auto newCopy = std::make_unique<BlockMap<WIDTH, HEIGHT>>();
        this->each([&](int x, int y, int z, char w){
            newCopy->set(x,y,z,w);
        });
        return newCopy;
    };

    constexpr int width() { return WIDTH; }
    constexpr int height() { return HEIGHT; }
};

typedef BlockMap<CHUNK_SIZE * 3, CHUNK_HEIGHT> BigBlockMap;

#endif
