#ifndef _map_h_
#define _map_h_

#define EMPTY_ENTRY(entry) ((entry)->value == 0)

#include <functional>


typedef union {
    unsigned int value;
    struct {
        unsigned char x;
        unsigned char y;
        unsigned char z;
        char w;
    } e;
} MapEntry;

class Map {
private:
    MapEntry *data;

public:
    int dx;
    int dy;
    int dz;
    unsigned int mask;
    unsigned int size;

    Map(int dx, int dy, int dz, unsigned int mask);

    ~Map();

    Map() = delete;
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
    Map(Map&&) = delete;
    Map& operator=(Map&&) = delete;

    Map* clone();

    int set(int x, int y, int z, int w);
    int get(int x, int y, int z);
    void each(std::function<void (int, int, int, int)>);
    void grow();
};

#endif
