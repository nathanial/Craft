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
class VisualChunk;

class Chunk {
private:
    const std::unique_ptr<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>> blocks;
    const int _p;
    const int _q; // chunk position
public:

    Chunk(int p, int q) : _p(p), _q(q),
    blocks(new BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>())
    {
    }


    int set_block(int x, int y, int z, char w);

    std::shared_ptr<VisualChunk> load() const;
    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    void foreach_block(std::function<void (int, int, int, char)> func) const;
    int p() const;
    int q() const;
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
