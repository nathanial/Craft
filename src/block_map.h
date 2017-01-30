#ifndef _map_h_
#define _map_h_

#include <functional>
#include <cmath>
#include <unordered_map>
#include "config.h"

class BlockMap {
public:
    char _data[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE] = {{{0}}};

    BlockMap();

    ~BlockMap();

    BlockMap(const BlockMap&) = delete;
    BlockMap& operator=(const BlockMap&) = delete;
    BlockMap(BlockMap&&) = delete;
    BlockMap& operator=(BlockMap&&) = delete;

    int set(int x, int y, int z, char w);
    char get(int x, int y, int z);
    void each(std::function<void (int, int, int, char)>);
    unsigned int size() const;
};

#endif
