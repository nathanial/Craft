//
// Created by nathan on 2/1/17.
//

#ifndef CRAFT_GENERATE_CHUNK_TASK_H
#define CRAFT_GENERATE_CHUNK_TASK_H

#include "task.h"
#include "../../chunk.h"

class GenerateChunkTask : public Task<ChunkPtr> {
private:
    const int p;
    const int q;
public:

    GenerateChunkTask(int p, int q);
    std::future<ChunkPtr> run() override;
};

#endif //CRAFT_GENERATE_CHUNK_TASK_H
