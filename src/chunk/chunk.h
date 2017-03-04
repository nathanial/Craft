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
class ChunkMesh;
class Chunk;

typedef BlockMap<CHUNK_SIZE, CHUNK_HEIGHT> ChunkBlocks;
typedef std::tuple<int, int> ChunkPosition;

struct ChunkPositionHash : public std::unary_function<ChunkPosition, std::size_t>
{
    std::size_t operator()(const ChunkPosition& k) const
    {
        return std::get<0>(k) ^ (std::get<1>(k) << 1);
    }
};

typedef std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>, ChunkPositionHash> ChunkNeighbors;

class Chunk {
private:
    // mesh is threadsafe because ChunkMesh is immutable
    mutable std::mutex _mesh_mtx;
    std::shared_ptr<ChunkMesh> _mesh;
    mutable std::mutex _blocks_mtx;

public:
    const int p, q;
    std::shared_ptr<const ChunkBlocks> blocks;

    Chunk(int p, int q);
    ~Chunk();

    // THREAD SAFE
    void set_mesh(std::shared_ptr<ChunkMesh> data);
    std::shared_ptr<ChunkMesh> mesh() const;
    void set_blocks(const std::shared_ptr<const ChunkBlocks> blocks);
    int set_block(int x, int y, int z, char w);
    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    void foreach_block(std::function<void (int, int, int, char)> func) const;

    int distance(int p, int q) const;

public:
    // Make sure the function that's providing "blocks" holds the _block_mtx
    static std::shared_ptr<ChunkMesh> create_mesh(int _p, int _q, bool dirty, GLuint buffer, const ChunkBlocks &blocks,
                                                  const ChunkNeighbors &neighbors);

private:
    static void populate_light_array(int _p, int _q, BigBlockMap &opaque, BigBlockMap &light, const ChunkNeighbors& neighbors);
    static std::vector<GLfloat> generate_geometry(int p, int q, const ChunkBlocks& blocks, BigBlockMap &opaque, BigBlockMap &light, HeightMap<CHUNK_SIZE * 3> &highest);
    static std::tuple<int,int,int> count_faces(int p, int q, const ChunkBlocks& blocks, const BigBlockMap &opaque);
    static void populate_opaque_array(int _p, int _q, BigBlockMap &opaque, HeightMap<48> &highest, const ChunkNeighbors& neighbors);
};

typedef std::shared_ptr<Chunk> ChunkPtr;

int highest_block(float x, float z);
int chunk_visible(arma::mat planes, int p, int q, int miny, int maxy);

#endif //CRAFT_CHUNK_H
