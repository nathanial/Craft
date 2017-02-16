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
#include <vector>

class Attrib;

class Chunk {
private:
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;
    int _p, _q; // chunk position
    int _faces;
    bool _dirty;
    int _miny;
    int _maxy;
    GLuint _buffer;
    std::vector<GLfloat> _vertices;
public:


    Chunk(int p, int q);
    ~Chunk();

    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    int set_block(int x, int y, int z, char w);

    int distance(int p, int q);
    void set_dirty_flag();

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

    const std::vector<GLfloat> vertices() const;
    void set_vertices(std::vector<GLfloat> vertices);

    void generate_buffer();

    bool is_ready_to_draw() const;

    void load();

    void populate_opaque_array(BigBlockMap &opaque, HeightMap<48> &highest, int ox, int oy, int oz) const;

    void populate_light_array(BigBlockMap &opaque, BigBlockMap &light, int ox, int oy, int oz) const;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

int highest_block(float x, float z);
int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
