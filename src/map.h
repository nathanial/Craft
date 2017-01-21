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
public:
    int dx;
    int dy;
    int dz;
    unsigned int mask;
    unsigned int size;
    MapEntry *data;

    Map(int dx, int dy, int dz, unsigned int mask);

    Map() = delete;

    ~Map();

    Map(const Map&) = delete;


    Map& operator=(const Map&) = delete;

    // no move
    Map(Map&&) = delete;

    // no move-assign
    Map& operator=(Map&&) = delete;

    Map* clone();
};

void map_grow(Map *map);
int map_set(Map *map, int x, int y, int z, int w);
int map_get(const Map *map, int x, int y, int z);

void map_for_each(Map *map, std::function<void (int, int, int, int)>);

#endif
