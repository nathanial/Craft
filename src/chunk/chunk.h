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
#include <vector>
#include <armadillo>

class Attrib;

class ChunkRenderData {
public:
    int faces;
    bool dirty;
    int miny;
    int maxy;
    GLuint buffer;
    std::vector<GLfloat> vertices;

    ChunkRenderData(){}
    ChunkRenderData(int miny, int maxy, int faces, bool dirty, GLuint buffer, const std::vector<GLfloat> &vertices) :
            miny(miny), maxy(maxy), faces(faces), dirty(dirty), buffer(buffer), vertices(vertices) {}
};

class Chunk {
private:
    std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;
    int _p, _q; // chunk position
public:
    ChunkRenderData render_data;
    Chunk(int p, int q);
    ~Chunk();

    void generate_buffer();
    int set_block(int x, int y, int z, char w);
    void set_dirty_flag();

    int draw(Attrib *attrib) const;
    ChunkRenderData load() const;
    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    void foreach_block(std::function<void (int, int, int, char)> func) const;
    int p() const;
    int q() const;
    void set_dirty(bool dirty);
    bool is_ready_to_draw() const;
    const std::vector<GLfloat> vertices() const;
    int distance(int p, int q) const;
    void populate_opaque_array(BigBlockMap &opaque, HeightMap<48> &highest) const;
    void populate_light_array(BigBlockMap &opaque, BigBlockMap &light) const;
    std::tuple<int,int,int> count_faces(BigBlockMap &opaque) const;
    std::vector<GLfloat> generate_geometry(BigBlockMap &opaque, BigBlockMap &light, HeightMap<CHUNK_SIZE * 3> &highest) const;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

int highest_block(float x, float z);
int chunk_visible(arma::mat planes, int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
