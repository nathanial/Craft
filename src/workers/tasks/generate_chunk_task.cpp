
#include "generate_chunk_task.h"

GenerateChunkTask::GenerateChunkTask(int p, int q) : p(p), q(q) {

}

std::future<ChunkPtr> GenerateChunkTask::run() {
    return std::async(std::launch::async, [&](){
        std::vector<char> blocks;
        blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT);
        std::fill(blocks.begin(), blocks.end(), 0);
        return std::make_shared<Chunk>(this->p, this->q, blocks);
    });
}
