#ifndef _map_h_
#define _map_h_

#include <functional>
#include <cmath>
#include <unordered_map>
#include <vector>
#include "config.h"


template<unsigned int WIDTH, unsigned int HEIGHT>
class BlockMap {
public:
    std::vector<char> _data;

    BlockMap() {
        _data.resize(WIDTH * WIDTH * HEIGHT);
        std::fill(_data.begin(), _data.end(), 0);
    }

    ~BlockMap() {}

    BlockMap(const BlockMap&) = delete;
    BlockMap& operator=(const BlockMap&) = delete;
    BlockMap(BlockMap&&) = delete;
    BlockMap& operator=(BlockMap&&) = delete;

    int set(int x, int y, int z, char w) {
        if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || z < 0 || z >= WIDTH){
            printf("Bad Index %d,%d,%d\n", x, y, z);
            throw new std::invalid_argument("Bad Index");
        }

        int overwrite = 0;
        if(this->_data[BLOCK_INDEX(x,y,z)] > 0){
            overwrite = 1;
        }
        this->_data[BLOCK_INDEX(x,y,z)] = w;
        if(w){
            return 1;
        }
        return overwrite;
    }
    char get(int x, int y, int z) const {
        if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || z < 0 || z >= WIDTH){
            printf("Bad Index %d,%d,%d\n", x, y, z);
            throw new std::invalid_argument("Bad Index");
        }
        return this->_data[BLOCK_INDEX(x,y,z)];
    }
    char get_or_default(int x, int y, int z, char _default) const {
        if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT|| z < 0 || z >= WIDTH){
            return _default;
        }
        return this->_data[BLOCK_INDEX(x,y,z)];
    }
    void each(std::function<void (int, int, int, char)> func) {
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                for(int z = 0; z < WIDTH; z++){
                    func(x, y, z, this->_data[BLOCK_INDEX(x,y,z)]);
                }
            }
        }
    }
    void each(std::function<void (int, int, int, char)> func) const {
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                for(int z = 0; z < WIDTH; z++){
                    func(x, y, z, this->_data[BLOCK_INDEX(x,y,z)]);
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

private:
    static inline int BLOCK_INDEX(int x, int y, int z){
        return x * WIDTH * HEIGHT + y * WIDTH + z;
    }
};

typedef BlockMap<CHUNK_SIZE * 3, CHUNK_HEIGHT> BigBlockMap;

#endif
