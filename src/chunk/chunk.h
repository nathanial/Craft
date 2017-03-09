#ifndef CRAFT_CHUNK_H
#define CRAFT_CHUNK_H

#include <GL/glew.h>
#include "../block_map.h"
#include "../height_map.h"
#include "../sign.h"
#include "TransientChunk.h"
#include <memory>
#include <mutex>
#include <vector>
#include <armadillo>
#include <caf/allowed_unsafe_message_type.hpp>


class Attrib;
class ChunkMesh;
class Chunk;
class TransientChunkMesh;

typedef BlockMap<CHUNK_SIZE, CHUNK_HEIGHT> ChunkBlocks;
typedef std::tuple<int, int> ChunkPosition;

struct ChunkPositionHash : public std::unary_function<ChunkPosition, std::size_t>
{
    std::size_t operator()(const ChunkPosition& k) const
    {
        return std::get<0>(k) ^ (std::get<1>(k) << 1);
    }
};

using ChunkNeighbors = std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>, ChunkPositionHash>;

class Chunk {
public:
    const int p, q;
    const std::unique_ptr<const ChunkBlocks> blocks;

    Chunk(int p, int q);
    Chunk(int p, int q, std::unique_ptr<ChunkBlocks> blocks);
    Chunk(const Chunk& other);
    ~Chunk();

    // THREAD SAFE
    int distance(int p, int q) const;
    std::shared_ptr<TransientChunk> transient() const;
    int get_block(int x, int y, int z) const;
    int get_block_or_zero(int x, int y, int z) const;
    void foreach_block(std::function<void (int, int, int, char)> func) const;
    static void create_mesh(int _p, int _q, TransientChunkMesh &mesh, const ChunkBlocks &blocks,
                                                  const ChunkNeighbors &neighbors);

private:
    static void populate_light_array(int _p, int _q, BigBlockMap &opaque, BigBlockMap &light, const ChunkNeighbors& neighbors);
    static std::vector<GLfloat> generate_geometry(int p, int q, const ChunkBlocks& blocks, BigBlockMap &opaque, BigBlockMap &light, HeightMap<CHUNK_SIZE * 3> &highest);
    static std::tuple<int,int,int> count_faces(int p, int q, const ChunkBlocks& blocks, const BigBlockMap &opaque);
    static void populate_opaque_array(int _p, int _q, BigBlockMap &opaque, HeightMap<48> &highest, const ChunkNeighbors& neighbors);
};

using ChunkPtr = std::shared_ptr<Chunk>;

int highest_block(float x, float z);
int chunk_visible(arma::mat planes, int p, int q, int miny, int maxy);

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(Chunk);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkPtr);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(ChunkNeighbors);

#endif //CRAFT_CHUNK_H
