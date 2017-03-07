//
// Created by nathan on 1/17/17.
//

extern "C" {
    #include <noise.h>
}
#include <armadillo>
#include <queue>
#include "chunk.h"
#include "../util.h"
#include "../item.h"
#include "../draw.h"
#include "../cube.h"
#include "ChunkMesh.h"
#include "../model.h"

void occlusion(
        char neighbors[27], char lights[27], float shades[27],
        float ao[6][4], float light[6][4]);

void scanline_iterate(BigBlockMap &light, BigBlockMap &opaque, std::deque<std::tuple<int, int, int, int>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, bool ascend);

void light_fill_scanline(BigBlockMap &opaque, BigBlockMap &light, int ox, int oy ,int oz, int ow);

Chunk::Chunk(int p, int q) :
    p(p), q(q),
    blocks(std::make_unique<BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>>())
{
}


Chunk::Chunk(int p, int q, std::unique_ptr<ChunkBlocks> blocks)
: p(p),q(q), blocks(blocks->copy())
{
}

Chunk::Chunk(const Chunk& other) : p(other.p), q(other.q), blocks(other.blocks->copy()) {

}


Chunk::~Chunk() {
    // del_buffer(this->mesh()->buffer);
}

int Chunk::get_block(int x, int y, int z) const {
    return this->blocks->get(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE);
}

int Chunk::get_block_or_zero(int x, int y, int z) const {
    return this->blocks->get_or_default(x - this->p * CHUNK_SIZE, y, z - this->q * CHUNK_SIZE, 0);
}

void Chunk::foreach_block(std::function<void (int, int, int, char)> func) const {
    this->blocks->each([&](int x, int y, int z, char w){
        func(x + this->p * CHUNK_SIZE, y, z + this->q * CHUNK_SIZE, w);
    });
}

std::shared_ptr<TransientChunk> Chunk::transient() const {
    auto t = std::make_shared<TransientChunk>(p, q);
    t->blocks = this->blocks->copy();
    return t;
}

int Chunk::distance(int p, int q) const {
    int dp = ABS(this->p - p);
    int dq = ABS(this->q - q);
    return MAX(dp, dq);
}

std::tuple<int,int,int> Chunk::count_faces(int p, int q, const ChunkBlocks& blocks, const BigBlockMap &opaque) {
    int ox = p * CHUNK_SIZE - CHUNK_SIZE;
    int oy = -1;
    int oz = q * CHUNK_SIZE - CHUNK_SIZE;

    int miny = 256;
    int maxy = 0;
    int faces = 0;
    blocks.each([&](int ex, int ey, int ez, int ew) {
        if (ew <= 0) {
            return;
        }
        ex += p * CHUNK_SIZE;
        ez += q * CHUNK_SIZE;
        int x = ex - ox;
        int y = ey - oy;
        int z = ez - oz;
        int f1 = !opaque.get(x - 1, y, z);
        int f2 = !opaque.get(x + 1, y, z);
        int f3 = !opaque.get(x, y + 1, z);
        int f4 = !opaque.get(x, y - 1, z) && (ey > 0);
        int f5 = !opaque.get(x, y, z - 1);
        int f6 = !opaque.get(x, y, z + 1);
        int total = f1 + f2 + f3 + f4 + f5 + f6;
        if (total == 0) {
            return;
        }
        if (is_plant(ew)) {
            total = 4;
        }
        miny = MIN(miny, ey);
        maxy = MAX(maxy, ey);
        faces += total;
    });
    return std::make_tuple(miny,maxy,faces);
};

std::vector<GLfloat> Chunk::generate_geometry(int p, int q, const ChunkBlocks &blocks,  BigBlockMap &opaque, BigBlockMap &light, HeightMap<CHUNK_SIZE * 3> &highest) {
    int ox = p * CHUNK_SIZE - CHUNK_SIZE;
    int oy = -1;
    int oz = q * CHUNK_SIZE - CHUNK_SIZE;

    std::vector<GLfloat> data;
    int offset = 0;
    blocks.each([&](int ex, int ey, int ez, int ew) {
        if (ew <= 0) {
            return;
        }
        ex += p * CHUNK_SIZE;
        ez += q * CHUNK_SIZE;
        int x = ex - ox;
        int y = ey - oy;
        int z = ez - oz;
        int f1 = !opaque.get(x - 1, y, z);
        int f2 = !opaque.get(x + 1, y, z);
        int f3 = !opaque.get(x, y + 1, z);
        int f4 = !opaque.get(x, y - 1, z) && (ey > 0);
        int f5 = !opaque.get(x, y, z - 1);
        int f6 = !opaque.get(x, y, z + 1);
        int total = f1 + f2 + f3 + f4 + f5 + f6;
        if (total == 0) {
            return;
        }
        char neighbors[27] = {0};
        char lights[27] = {0};
        float shades[27] = {0};
        int index = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dz = -1; dz <= 1; dz++) {
                    neighbors[index] = opaque.get(x + dx, y + dy, z + dz);
                    lights[index] = light.get(x + dx, y + dy, z + dz);
                    shades[index] = 0;
                    if (y + dy <= highest.get(x + dx, z + dz)) {
                        for (int oy = 0; oy < 8; oy++) {
                            if (opaque.get(x + dx, y + dy + oy, z + dz)) {
                                shades[index] = 1.0 - oy * 0.125;
                                break;
                            }
                        }
                    }
                    index++;
                }
            }
        }
        float ao[6][4];
        float light[6][4];
        occlusion(neighbors, lights, shades, ao, light);
        if (is_plant(ew)) {
            total = 4;
            float min_ao = 1;
            float max_light = 0;
            for (int a = 0; a < 6; a++) {
                for (int b = 0; b < 4; b++) {
                    min_ao = MIN(min_ao, ao[a][b]);
                    max_light = MAX(max_light, light[a][b]);
                }
            }
            float rotation = simplex2(ex, ez, 4, 0.5, 2) * 360;
            add_all(data, make_plant(min_ao, max_light, ex, ey, ez, 0.5, ew, rotation));
        }
        else {
            add_all(data, make_cube(ao, light, f1, f2, f3, f4, f5, f6, ex, ey, ez, 0.5, ew));
        }
        offset += total * 60;
    });
    return data;
}


void Chunk::create_mesh(int _p, int _q, TransientChunkMesh &mesh, const ChunkBlocks &blocks, const ChunkNeighbors &neighbors) {
    auto opaque = std::make_unique<BigBlockMap>();
    auto light = std::make_unique<BigBlockMap>();
    auto highest = std::make_unique<HeightMap<CHUNK_SIZE * 3>>();

    Chunk::populate_opaque_array(_p, _q, *opaque, *highest, neighbors);
    Chunk::populate_light_array(_p, _q, *opaque, *light, neighbors);

    int miny, maxy, faces;
    std::tie(miny, maxy, faces) = Chunk::count_faces(_p, _q, blocks, *opaque);
    auto data = Chunk::generate_geometry(_p, _q, blocks, *opaque, *light, *highest);

    mesh.miny = miny;
    mesh.maxy = maxy;
    mesh.faces = faces;
    mesh.vertices = data;
}

void Chunk::populate_light_array(int _p, int _q, BigBlockMap &opaque, BigBlockMap &light, const ChunkNeighbors& neighbors) {
    int ox = _p * CHUNK_SIZE - CHUNK_SIZE;
    int oy = -1;
    int oz = _q * CHUNK_SIZE - CHUNK_SIZE;

    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            auto chunk = neighbors.at(std::make_tuple(_p - (a - 1), _q - (b - 1)));
            if(chunk){
                int chunk_x_offset = chunk->p * CHUNK_SIZE;
                int chunk_z_offset = chunk->q * CHUNK_SIZE;
                for(int bx = 0; bx < CHUNK_SIZE; bx++){
                    for(int by = 0; by < CHUNK_HEIGHT; by++) {
                        for (int bz = 0; bz < CHUNK_SIZE; bz++) {
                            int ex = bx + chunk_x_offset;
                            int ey = by;
                            int ez = bz + chunk_z_offset;
                            int ew = chunk->blocks->_data[bx][by][bz];
                            if(ew == 0){
                                continue;
                            }

                            if(ey == CHUNK_HEIGHT){
                                continue;
                            }

                            int lx = ex - ox;
                            int ly = ey - oy;
                            int lz = ez - oz;

                            if (is_light(ew)) {
                                light_fill_scanline(opaque, light, lx, ly, lz, 15);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Chunk::populate_opaque_array(int _p, int _q, BigBlockMap &opaque, HeightMap<48> &highest, const ChunkNeighbors &neighbors) {
    int ox = _p * CHUNK_SIZE - CHUNK_SIZE;
    int oy = -1;
    int oz = _q * CHUNK_SIZE - CHUNK_SIZE;
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            auto chunk = neighbors.at(std::make_tuple(_p + (a - 1), _q + (b - 1)));
            if(!chunk){
                continue;
            }
            int chunk_x_offset = chunk->p * CHUNK_SIZE;
            int chunk_z_offset = chunk->q * CHUNK_SIZE;
            for(int bx = 0; bx < CHUNK_SIZE; bx++){
                for(int by = 0; by < CHUNK_HEIGHT; by++){
                    for(int bz = 0; bz < CHUNK_SIZE; bz++){
                        int ex = bx + chunk_x_offset;
                        int ey = by;
                        int ez = bz + chunk_z_offset;
                        int ew = chunk->blocks->_data[bx][by][bz];
                        if(ew == 0){
                            continue;
                        }

                        int x = ex - ox;
                        int y = ey - oy;
                        int z = ez - oz;
                        if(y == CHUNK_HEIGHT){
                            continue;
                        }
                        bool is_opaque = !is_transparent(ew) && !is_light(ew);
                        opaque._data[x][y][z] = is_opaque ;
                        if (is_opaque) {
                            highest._data[x][z] = MAX(highest._data[x][z], y);
                        }
                    }
                }
            }
        }
    }
}

int chunk_visible(arma::mat planes, int p, int q, int miny, int maxy) {
    int x = p * CHUNK_SIZE - 1;
    int z = q * CHUNK_SIZE - 1;
    int d = CHUNK_SIZE + 1;
    float points[8][3] = {
            {x + 0, miny, z + 0},
            {x + d, miny, z + 0},
            {x + 0, miny, z + d},
            {x + d, miny, z + d},
            {x + 0, maxy, z + 0},
            {x + d, maxy, z + 0},
            {x + 0, maxy, z + d},
            {x + d, maxy, z + d}
    };
    int n = 6;
    for (int i = 0; i < n; i++) {
        int in = 0;
        int out = 0;
        for (int j = 0; j < 8; j++) {
            float d =
                    planes(i,0) * points[j][0] +
                    planes(i,1) * points[j][1] +
                    planes(i,2) * points[j][2] +
                    planes(i,3);
            if (d < 0) {
                out++;
            }
            else {
                in++;
            }
            if (in && out) {
                break;
            }
        }
        if (in == 0) {
            return 0;
        }
    }
    return 1;
}

extern Model *g;
int highest_block(float x, float z) {
    int result = -1;
    int nx = roundf(x);
    int nz = roundf(z);
    int p = chunked(x);
    int q = chunked(z);
    auto chunk = g->find_chunk(p, q);
    if (chunk) {
        chunk->foreach_block([&](int ex, int ey, int ez, int ew){
            if (is_obstacle(ew) && ex == nx && ez == nz) {
                result = MAX(result, ey);
            }
        });
    }
    return result;
}


void light_fill_scanline(BigBlockMap &opaque, BigBlockMap &light, int ox, int oy ,int oz, int ow)
{
    std::deque<std::tuple<int,int,int,int>> frontier;
    frontier.push_back(std::make_tuple(ox,oy,oz,ow));
    while(!frontier.empty()){
        auto &next = frontier.front();
        int x = std::get<0>(next);
        int y = std::get<1>(next);
        int z = std::get<2>(next);
        int w = std::get<3>(next);
        frontier.pop_front();

        if(w == 0){
            continue;
        }
        if(opaque.get(x,y,z)){
            continue;
        }
        if(light.get(x,y,z) >= w){
            continue;
        }

        int cursorX = x;
        int cursorW = w;
        scanline_iterate(light, opaque, frontier, x, y, z, w, cursorX, cursorW, true);
        scanline_iterate(light, opaque, frontier, x, y, z, w, cursorX-1, cursorW, false);
    }
}



void scanline_iterate(BigBlockMap &light, BigBlockMap &opaque, std::deque<std::tuple<int, int, int, int>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, bool ascend) {

    auto canLight = [&](int x, int y, int z, int w){
        return light.get(x, y, z) < w && !opaque.get(x, y, z);
    };

    bool spanZMinus = false, spanZPlus = false, spanYMinus = false, spanYPlus = false;
    while(cursorX < CHUNK_SIZE * 3 && cursorX >= 0 && canLight(cursorX, y, z, w - ABS(x - cursorX))){
        cursorW = w - ABS(x - cursorX);
        light.set(cursorX, y, z, cursorW);
        if(!spanZMinus && z > 0 && canLight(cursorX, y, z-1, cursorW-1)) {
            frontier.push_back(std::make_tuple(cursorX, y, z - 1, cursorW - 1));
            spanZMinus = true;
        } else if(spanZMinus && z > 0 && !canLight(cursorX, y, z - 1, cursorW-1)){
            spanZMinus = false;
        }
        if(!spanZPlus && z < CHUNK_SIZE * 3 - 1 && canLight(cursorX, y, z+1, cursorW - 1)){
            frontier.push_back(std::make_tuple(cursorX, y, z + 1, cursorW - 1));
            spanZPlus = true;
        } else if(spanZPlus && z < CHUNK_SIZE * 3 - 1 && !canLight(cursorX, y, z + 1, cursorW - 1)){
            spanZPlus = false;
        }
        if(!spanYMinus && y > 0 && canLight(cursorX, y-1,z, cursorW-1)){
            frontier.push_back(std::make_tuple(cursorX, y-1, z, cursorW-1));
            spanYMinus = true;
        } else if(spanYMinus && y > 0 && !canLight(cursorX, y-1,z, cursorW- 1)){
            spanYMinus = false;
        }
        if(!spanYPlus && y < CHUNK_HEIGHT - 1 && canLight(cursorX, y+1,z, cursorW-1)){
            frontier.push_back(std::make_tuple(cursorX, y+1,z, cursorW-1));
            spanYPlus = true;
        } else if(spanYPlus && y < CHUNK_HEIGHT -1 && !canLight(cursorX, y+1, z, cursorW-1)){
            spanYPlus = false;
        }

        if(ascend){
            cursorX++;
        } else {
            cursorX--;
        }

    }
}


static float light_levels[] = {
        0.5629499534213125f, 0.7036874417766406f, 0.8796093022208006f, 1.0995116277760006f, 1.3743895347200008f,
        1.717986918400001f, 2.147483648000001f, 2.6843545600000014f, 3.3554432000000016f, 4.194304000000002f,
        5.242880000000002f, 6.553600000000002f, 8.192000000000002f, 10.240000000000002f, 12.8f, 16.0f
};


float get_light_level(int level){
    if(level < 0 || level > 15) {
        throw "Bad Light Level";
    }
    return light_levels[level];
}

void occlusion(
        char neighbors[27], char lights[27], float shades[27],
        float ao[6][4], float light[6][4])
{
    static const int lookup3[6][4][3] = {
            {{0, 1, 3}, {2, 1, 5}, {6, 3, 7}, {8, 5, 7}},
            {{18, 19, 21}, {20, 19, 23}, {24, 21, 25}, {26, 23, 25}},
            {{6, 7, 15}, {8, 7, 17}, {24, 15, 25}, {26, 17, 25}},
            {{0, 1, 9}, {2, 1, 11}, {18, 9, 19}, {20, 11, 19}},
            {{0, 3, 9}, {6, 3, 15}, {18, 9, 21}, {24, 15, 21}},
            {{2, 5, 11}, {8, 5, 17}, {20, 11, 23}, {26, 17, 23}}
    };
    static const int lookup4[6][4][4] = {
            {{0, 1, 3, 4}, {1, 2, 4, 5}, {3, 4, 6, 7}, {4, 5, 7, 8}},
            {{18, 19, 21, 22}, {19, 20, 22, 23}, {21, 22, 24, 25}, {22, 23, 25, 26}},
            {{6, 7, 15, 16}, {7, 8, 16, 17}, {15, 16, 24, 25}, {16, 17, 25, 26}},
            {{0, 1, 9, 10}, {1, 2, 10, 11}, {9, 10, 18, 19}, {10, 11, 19, 20}},
            {{0, 3, 9, 12}, {3, 6, 12, 15}, {9, 12, 18, 21}, {12, 15, 21, 24}},
            {{2, 5, 11, 14}, {5, 8, 14, 17}, {11, 14, 20, 23}, {14, 17, 23, 26}}
    };
    static const float curve[4] = {0.0, 0.25, 0.5, 0.75};
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            int corner = neighbors[lookup3[i][j][0]];
            int side1 = neighbors[lookup3[i][j][1]];
            int side2 = neighbors[lookup3[i][j][2]];
            int value = side1 && side2 ? 3 : corner + side1 + side2;
            float shade_sum = 0;
            float light_sum = 0;
            int is_light = lights[13] == 15;
            for (int k = 0; k < 4; k++) {
                shade_sum += shades[lookup4[i][j][k]];
                light_sum += get_light_level(lights[lookup4[i][j][k]]);
            }
            if (is_light) {
                light_sum = 15 * 4 * 10;
            }
            float total = (curve[value] + shade_sum / 4.0) * 0.01;
            ao[i][j] = MIN(total, 1.0);
            light[i][j] = light_sum / 15.0 / 4.0;
        }
    }
}
