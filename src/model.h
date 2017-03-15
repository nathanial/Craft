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
    float fov;
    int day_length;
    Model();
};

#endif //CRAFT_MODEL_H
