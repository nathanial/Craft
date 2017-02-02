
#include "generate_chunk_task.h"

GenerateChunkTask::GenerateChunkTask(int p, int q) : p(p), q(q) {

}

std::future<ChunkPtr> GenerateChunkTask::run() {
    return std::async(std::launch::async, [&](){
        return std::make_shared<Chunk>(this->p, this->q);
    });
}
