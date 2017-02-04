//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/glew.h>
#include "block_map.h"
#include "height_map.h"
#include "sign.h"
#include <memory>
#include <mutex>
#include <shared_mutex>

class Attrib;

class NeighborEdges {
public:
    std::shared_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, 1>> north_edge;
    std::shared_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, 1>> south_edge;
    std::shared_ptr<BlockMap<1, CHUNK_HEIGHT, CHUNK_SIZE>> west_edge;
    std::shared_ptr<BlockMap<1, CHUNK_HEIGHT, CHUNK_SIZE>> east_edge;
};

typedef std::shared_ptr<NeighborEdges> NeighborEdgesPtr;

class Chunk {
private:

    int _p, _q; // chunk position
    int _faces;
    bool _dirty;
    int _miny;
    int _maxy;
    GLuint _buffer;
    GLfloat *_vertices;
public:
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE>> blocks;

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

    void load(NeighborEdgesPtr neighbor_edges);

    void redraw(NeighborEdgesPtr edges);
};

typedef std::shared_ptr<Chunk> ChunkPtr;

int highest_block(float x, float z);
int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
