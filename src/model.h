//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_MODEL_H
#define CRAFT_MODEL_H

#include <GL/glew.h>
#include "chunk/chunk.h"
#include "config.h"
#include "actors/Actors.h"
#include <functional>
#include <tuple>
#include <vector>
#include <GLFW/glfw3.h>
#include "chunk/TransientChunk.h"
#include "chunk/TransientChunkMesh.h"

class Block {
public:
    int x;
    int y;
    int z;
    int w;
};

class State {
public:
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float t;
};

class Attrib {
public:
    GLuint program;
    GLuint position;
    GLuint normal;
    GLuint uv;
    GLuint matrix;
    GLuint sampler;
    GLuint camera;
    GLuint timer;
    GLuint extra1;
    GLuint extra2;
    GLuint extra3;
    GLuint extra4;
};

class Player {
public:
    int id;
    char name[MAX_NAME_LENGTH];
    State state;
    State state1;
    State state2;
    GLuint buffer;
};

class Model {
private:
    std::unordered_map<ChunkPosition, ChunkPtr, ChunkPositionHash> chunks;
    std::unordered_map<ChunkPosition, std::shared_ptr<ChunkMesh>, ChunkPositionHash> meshes;
public:
    GLFWwindow *window;
    int render_radius;
    int delete_radius;
    Player player;
    int width;
    int height;
    int flying;
    int item_index;
    int scale;
    int ortho;
    float fov;
    char db_path[MAX_PATH_LENGTH];
    int day_length;

    ChunkPtr get_chunk(int p, int q);
    std::shared_ptr<ChunkMesh> get_mesh(int p, int q);
    void clear_chunks();
    ChunkPtr find_chunk(int p, int q);
    void replace_chunk(ChunkPtr chunk);
    void update_chunk(int p, int q, std::function<void (TransientChunk&) > func);
    void update_mesh(int p, int q, std::function<void (TransientChunkMesh&) > func);
    std::shared_ptr<ChunkMesh> find_mesh(int p, int q);
    void replace_mesh(int p, int q, std::shared_ptr<ChunkMesh> mesh);

    void each_chunk(std::function<void (Chunk& chunk)>);
    void delete_chunks();
    void delete_all_chunks();

    int chunk_count() const;

    char get_block(int x, int y, int z);

    Model();
};

#endif //CRAFT_MODEL_H
