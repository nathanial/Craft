#ifndef _map_h_
#define _map_h_

#define EMPTY_ENTRY(entry) ((entry)->value == 0)

#include <functional>
#include <unordered_map>
#include <cmath>

struct MapEntry {
    unsigned short x;
    unsigned short y;
    unsigned short z;

    bool operator==(const MapEntry &other) const
    {
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }
};

namespace std {

    template <>
    struct hash<MapEntry>
    {
        std::size_t operator()(const MapEntry& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((hash<int>()(k.x)
                     ^ (hash<int>()(k.y) << 1)) >> 1)
                   ^ (hash<int>()(k.z) << 1);
        }
    };

}

class Map {
private:
    std::unordered_map<MapEntry, char> _data;
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
