//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/glew.h>
#include "../block_map.h"
#include "../height_map.h"
#include "../sign.h"
#include <memory>
#include <mutex>

class Attrib;
class Chunk;

typedef std::shared_ptr<Chunk> ChunkPtr;
typedef std::tuple<int, int> ChunkPosition;

struct ChunkPositionHash : public std::unary_function<ChunkPosition, std::size_t>
{
    std::size_t operator()(const ChunkPosition& k) const
    {
        return std::get<0>(k) ^ (std::get<1>(k) << 1);
    }
};
typedef std::unordered_map<ChunkPosition, ChunkPtr, ChunkPositionHash> Neighborhood;


class Chunk {
private:
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;
    int _p, _q; // chunk position
    int _faces;
    bool _dirty;
    int _miny;
    int _maxy;
    GLuint _buffer;
    GLfloat *_vertices;


    void populate_opaque_array(const Neighborhood &neighborhood, BigBlockMap *opaque, HeightMap<48> *highest, int ox, int oy, int oz) const;
    void populate_light_array(const Neighborhood &neighborhood, BigBlockMap *opaque, BigBlockMap *light, int ox, int oy, int oz) const;
public:


    Chunk(int p, int q);
    ~Chunk();

    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    int set_block(int x, int y, int z, char w);

    int distance(int p, int q);

    void foreach_block(std::function<void (int, int, int, char)> func);
    int draw(Attrib *attrib);

    int p() const;
    int q() const;

    void set_faces(int faces);
    int faces() const;

    void set_dirty(bool dirty);
    bool dirty() const;

    void set_maxy(int maxy);
    void set_miny(int miny);

    int maxy() const;
    int miny() const;

    GLfloat* vertices() const;
    void set_vertices(GLfloat *vertices);

    void generate_buffer();

    bool is_ready_to_draw() const;

    void load(const Neighborhood& neighborhood);

};


int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
