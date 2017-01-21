#ifndef _map_h_
#define _map_h_

#define EMPTY_ENTRY(entry) ((entry)->value == 0)

#include <functional>
#include <map>
#include <cmath>

typedef union {
    long long unsigned int value;
    struct {
        unsigned short x;
        unsigned short y;
        unsigned short z;
    } e;
} MapEntry;

struct cmpByDistance {
    bool operator()(const MapEntry& a, const MapEntry& b) const {
        return a.value < b.value;
    }
};

class Map {
private:
    std::map<MapEntry, char, cmpByDistance> _data;
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
