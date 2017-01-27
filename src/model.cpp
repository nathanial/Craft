#include "model.h"
#include "chunk.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

ChunkPtr Model::get_chunk(int i) {
    return this->chunks + i;
}

void Model::clear_chunks(){
    memset(this->chunks, 0, sizeof(Chunk) * MAX_CHUNKS);
}

ChunkPtr Model::find_chunk(int p, int q) {
    for (int i = 0; i < this->chunk_count; i++) {
        auto chunk = this->get_chunk(i);
        if (chunk->p == p && chunk->q == q) {
            return chunk;
        }
    }
    return 0;
}

void Model::each_chunk(std::function<void (ChunkPtr chunk)> func) {
    for (int i = 0; i < this->chunk_count; i++) {
        auto chunk = this->get_chunk(i);
        func(chunk);
    }
}

void Model::delete_chunks(){
    int count = this->chunk_count;
    State *s1 = &this->players->state;
    State *s2 = &(this->players + this->observe1)->state;
    State *s3 = &(this->players + this->observe2)->state;
    State *states[3] = {s1, s2, s3};
    for (int i = 0; i < count; i++) {
        auto chunk = this->get_chunk(i);
        int _delete = 1;
        for (int j = 0; j < 3; j++) {
            State *s = states[j];
            int p = chunked(s->x);
            int q = chunked(s->z);
            if (chunk->distance(p, q) < this->delete_radius) {
                _delete = 0;
                break;
            }
        }
        if (_delete) {
            chunk->destroy();
            sign_list_free(&chunk->signs);
            del_buffer(chunk->buffer);
            del_buffer(chunk->sign_buffer);
            auto other = this->get_chunk(--count);
            memcpy(chunk, other, sizeof(Chunk));
        }
    }
    this->chunk_count = count;
}

void Model::delete_all_chunks() {
    for (int i = 0; i < this->chunk_count; i++) {
        auto chunk = this->get_chunk(i);
        chunk->destroy();
        sign_list_free(&chunk->signs);
        del_buffer(chunk->buffer);
        del_buffer(chunk->sign_buffer);
    }
    this->chunk_count = 0;
}