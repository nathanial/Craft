//
// Created by nathan on 1/17/17.
//

#ifndef CRAFT_MODEL_H
#define CRAFT_MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "chunk.h"
#include "config.h"
#include <functional>
#include <tuple>
#include <vector>
#include <queue>
#include <future>

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

typedef std::tuple<int, int> ChunkPosition;

struct ChunkPositionHash : public std::unary_function<ChunkPosition, std::size_t>
{
    std::size_t operator()(const ChunkPosition& k) const
    {
        return std::get<0>(k) ^ (std::get<1>(k) << 1);
    }
};

class Model {
private:
    std::unordered_map<ChunkPosition, ChunkPtr, ChunkPositionHash> chunks;
    std::queue<std::shared_future<ChunkPtr>> loading_chunks;
    std::unordered_map<ChunkPosition, bool, ChunkPositionHash> chunk_is_loading;

public:
    GLFWwindow *window;
    int create_radius;
    int render_radius;
    int delete_radius;
    Player players[MAX_PLAYERS];
    int player_count;
    int typing;
    char typing_buffer[MAX_TEXT_LENGTH];
    int message_index;
    char messages[MAX_MESSAGES][MAX_TEXT_LENGTH];
    int width;
    int height;
    int observe1;
    int observe2;
    int flying;
    int item_index;
    int scale;
    int ortho;
    float fov;
    int suppress_char;
    int mode;
    int mode_changed;
    char db_path[MAX_PATH_LENGTH];
    int day_length;
    int time_changed;
    Block block0;
    Block block1;
    Block copy0;
    Block copy1;

    ChunkPtr get_chunk(int p, int q);
    void clear_chunks();
    ChunkPtr find_chunk(int p, int q);

    void each_chunk(std::function<void (ChunkPtr chunk)>);
    void delete_chunks();
    void delete_all_chunks();

    int chunk_count() const;

    char get_block(int x, int y, int z);

    void draw_loaded_chunks();
    void request_chunk(int p, int q, bool force);
    void reload_chunk(int p, int q);

    NeighborEdgesPtr find_edges(int p, int q);

    Model();
};

#endif //CRAFT_MODEL_H
