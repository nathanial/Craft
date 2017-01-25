#ifndef _map_h_
#define _map_h_

#include <functional>
#include <cmath>
#include <unordered_map>
#include "config.h"

class Map {
private:
    char _data[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE] = {{{0}}};
public:
    int dx;
    int dy;
    int dz;

    Map(int dx, int dy, int dz);

    ~Map();

    Map() = delete;
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    Map(Map&&) = delete;
    Map& operator=(Map&&) = delete;

    Map* clone();

    int set(int x, int y, int z, char w);
    char get(int x, int y, int z);
    void each(std::function<void (int, int, int, char)>);
    unsigned int size() const;
};

#endif
