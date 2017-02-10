//
// Created by nathan on 1/17/17.
//

extern "C" {
    #include <noise.h>
}

#include <queue>
#include "chunk.h"
#include "util.h"
#include "item.h"
#include "matrix.h"
#include "draw.h"

void occlusion(
        char neighbors[27], char lights[27], float shades[27],
        float ao[6][4], float light[6][4]);

void make_plant(
        float *data, float ao, float light,
        float px, float py, float pz, float n, int w, float rotation);

void make_cube(
        float *data, float ao[6][4], float light[6][4],
        int left, int right, int top, int bottom, int front, int back,
        float x, float y, float z, float n, int w);

void scanline_iterate(BigBlockMap *light, BigBlockMap *opaque, std::deque<std::tuple<int, int, int, int>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, bool ascend);


void light_fill_scanline(BigBlockMap *opaque, BigBlockMap *light, int ox, int oy ,int oz, int ow);

Chunk::Chunk(int p, int q) :
    blocks(new BlockMap<CHUNK_SIZE, CHUNK_HEIGHT>())
{
    this->_p = p;
    this->_q = q;
    this->_faces = 0;
    this->_buffer = 0;
}

Chunk::~Chunk() {
    del_buffer(this->_buffer);
}

int Chunk::get_block(int x, int y, int z) const {
    return this->blocks->get(x - this->_p * CHUNK_SIZE, y, z - this->_q * CHUNK_SIZE);
}

int Chunk::get_block_or_zero(int x, int y, int z) const {
    return this->blocks->get_or_default(x - this->_p * CHUNK_SIZE, y, z - this->_q * CHUNK_SIZE, 0);
}

void Chunk::foreach_block(std::function<void (int, int, int, char)> func) {
    this->blocks->each([&](int x, int y, int z, char w){
        func(x + this->_p * CHUNK_SIZE, y, z + this->_q * CHUNK_SIZE, w);
    });
}

int Chunk::set_block(int x, int y, int z, char w){
    return this->blocks->set(x - this->_p * CHUNK_SIZE, y, z - this->_q * CHUNK_SIZE, w);
}

int Chunk::distance(int p, int q) {
    int dp = ABS(this->_p - p);
    int dq = ABS(this->_q - q);
    return MAX(dp, dq);
}

int Chunk::draw(Attrib *attrib) {
    if(this->_buffer){
        draw_triangles_3d_ao(attrib, this->_buffer, this->_faces * 6);
        return this->_faces;
    } else {
        return 0;
    }

}

int Chunk::p() const {
    return this->_p;
}

int Chunk::q() const {
    return this->_q;
}

void Chunk::set_faces(int faces) {
    this->_faces = faces;
}

int Chunk::faces() const {
    return this->_faces;
}

void Chunk::set_dirty(bool dirty) {
    this->_dirty = dirty;
}

bool Chunk::dirty() const {
    return this->_dirty;
}

void Chunk::set_maxy(int maxy) {
    this->_maxy = maxy;
}

void Chunk::set_miny(int miny) {
    this->_miny = miny;
}

int Chunk::maxy() const {
    return this->_maxy;
}

int Chunk::miny() const {
    return this->_miny;
}

GLfloat* Chunk::vertices() const {
    return this->_vertices;
}

void Chunk::set_vertices(GLfloat *vertices) {
    this->_vertices = vertices;
}

void Chunk::generate_buffer() {
    if(this->_buffer) {
        del_buffer(this->_buffer);
    }
    if(!this->_vertices){
        return;
    }
    this->_buffer = gen_faces(10, this->faces(), this->vertices());
    this->_vertices = nullptr;
}

bool Chunk::is_ready_to_draw() const {
    return this->_buffer && this->dirty();
}


void Chunk::load(const Neighborhood& neighborhood) {
    auto opaque = new BigBlockMap();
    auto light = new BigBlockMap();
    auto highest = new HeightMap<CHUNK_SIZE * 3>();

    int ox = this->_p * CHUNK_SIZE - CHUNK_SIZE;
    int oy = -1;
    int oz = this->_q * CHUNK_SIZE - CHUNK_SIZE;
    // printf("Compute Chunk %d,%d\n", this->_p, this->_q);

    // populate opaque array
    populate_opaque_array(neighborhood,  opaque, highest, ox, oy, oz);
    populate_light_array(neighborhood, opaque, light, ox, oy, oz);

    // count exposed faces
    int miny = 256;
    int maxy = 0;
    int faces = 0;
    this->foreach_block([&](int ex, int ey, int ez, int ew) {
        if (ew <= 0) {
            return;
        }
        int x = ex - ox;
        int y = ey - oy;
        int z = ez - oz;
        int f1 = !opaque->get(x - 1, y, z);
        int f2 = !opaque->get(x + 1, y, z);
        int f3 = !opaque->get(x, y + 1, z);
        int f4 = !opaque->get(x, y - 1, z) && (ey > 0);
        int f5 = !opaque->get(x, y, z - 1);
        int f6 = !opaque->get(x, y, z + 1);
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

    // generate geometry
    GLfloat *data = malloc_faces(10, faces);
    int offset = 0;
    this->foreach_block([&](int ex, int ey, int ez, int ew) {
        if (ew <= 0) {
            return;
        }
        int x = ex - ox;
        int y = ey - oy;
        int z = ez - oz;
        int f1 = !opaque->get(x - 1, y, z);
        int f2 = !opaque->get(x + 1, y, z);
        int f3 = !opaque->get(x, y + 1, z);
        int f4 = !opaque->get(x, y - 1, z) && (ey > 0);
        int f5 = !opaque->get(x, y, z - 1);
        int f6 = !opaque->get(x, y, z + 1);
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
                    neighbors[index] = opaque->get(x + dx, y + dy, z + dz);
                    lights[index] = light->get(x + dx, y + dy, z + dz);
                    shades[index] = 0;
                    if (y + dy <= highest->get(x + dx, z + dz)) {
                        for (int oy = 0; oy < 8; oy++) {
                            if (opaque->get(x + dx, y + dy + oy, z + dz)) {
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
            make_plant(
                    data + offset, min_ao, max_light,
                    ex, ey, ez, 0.5, ew, rotation);
        }
        else {
            make_cube(
                    data + offset, ao, light,
                    f1, f2, f3, f4, f5, f6,
                    ex, ey, ez, 0.5, ew);
        }
        offset += total * 60;
    });

    delete light;
    delete highest;
    delete opaque;

    this->set_miny(miny);
    this->set_maxy(maxy);
    this->set_faces(faces);
    this->set_vertices(data);
}

void Chunk::populate_light_array(const Neighborhood &neighborhood, BigBlockMap *opaque, BigBlockMap *light, int ox, int oy, int oz) const {
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            auto chunk = neighborhood.at(std::make_tuple(_p - (a - 1), _q - (b - 1)));
            if(chunk){
                int chunk_x_offset = chunk->_p * CHUNK_SIZE;
                int chunk_z_offset = chunk->_q * CHUNK_SIZE;
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

void Chunk::populate_opaque_array(const Neighborhood &neighborhood, BigBlockMap *opaque, HeightMap<48> *highest, int ox, int oy, int oz) const {
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            auto chunk = neighborhood.at(std::make_tuple(this->_p + (a - 1), this->_q + (b - 1)));
            if(!chunk){
                continue;
            }
            int chunk_x_offset = chunk->_p * CHUNK_SIZE;
            int chunk_z_offset = chunk->_q * CHUNK_SIZE;
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
                        opaque->_data[x][y][z] = is_opaque ;
                        if (is_opaque) {
                            highest->_data[x][z] = MAX(highest->_data[x][z], y);
                        }
                    }
                }
            }
        }
    }
}

int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy) {
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
                    planes[i][0] * points[j][0] +
                    planes[i][1] * points[j][1] +
                    planes[i][2] * points[j][2] +
                    planes[i][3];
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



void light_fill_scanline(BigBlockMap *opaque, BigBlockMap *light, int ox, int oy ,int oz, int ow)
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
        if(opaque->get(x,y,z)){
            continue;
        }
        if(light->get(x,y,z) >= w){
            continue;
        }

        int cursorX = x;
        int cursorW = w;
        scanline_iterate(light, opaque, frontier, x, y, z, w, cursorX, cursorW, true);
        scanline_iterate(light, opaque, frontier, x, y, z, w, cursorX-1, cursorW, false);
    }
}



void scanline_iterate(BigBlockMap *light, BigBlockMap *opaque, std::deque<std::tuple<int, int, int, int>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, bool ascend) {

    auto canLight = [&](int x, int y, int z, int w){
        return light->get(x, y, z) < w && !opaque->get(x, y, z);
    };

    bool spanZMinus = false, spanZPlus = false, spanYMinus = false, spanYPlus = false;
    while(cursorX < CHUNK_SIZE * 3 && cursorX >= 0 && canLight(cursorX, y, z, w - ABS(x - cursorX))){
        cursorW = w - ABS(x - cursorX);
        light->set(cursorX, y, z, cursorW);
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



void make_plant(
        float *data, float ao, float light,
        float px, float py, float pz, float n, int w, float rotation)
{
    static const float positions[4][4][3] = {
            {{ 0, -1, -1}, { 0, -1, +1}, { 0, +1, -1}, { 0, +1, +1}},
            {{ 0, -1, -1}, { 0, -1, +1}, { 0, +1, -1}, { 0, +1, +1}},
            {{-1, -1,  0}, {-1, +1,  0}, {+1, -1,  0}, {+1, +1,  0}},
            {{-1, -1,  0}, {-1, +1,  0}, {+1, -1,  0}, {+1, +1,  0}}
    };
    static const float normals[4][3] = {
            {-1, 0, 0},
            {+1, 0, 0},
            {0, 0, -1},
            {0, 0, +1}
    };
    static const float uvs[4][4][2] = {
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
            {{1, 0}, {0, 0}, {1, 1}, {0, 1}},
            {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
            {{1, 0}, {1, 1}, {0, 0}, {0, 1}}
    };
    static const float indices[4][6] = {
            {0, 3, 2, 0, 1, 3},
            {0, 3, 1, 0, 2, 3},
            {0, 3, 2, 0, 1, 3},
            {0, 3, 1, 0, 2, 3}
    };
    float *d = data;
    float s = 0.0625;
    float a = 0;
    float b = s;
    float du = (plants[w] % 16) * s;
    float dv = (plants[w] / 16) * s;
    for (int i = 0; i < 4; i++) {
        for (int v = 0; v < 6; v++) {
            int j = indices[i][v];
            *(d++) = n * positions[i][j][0];
            *(d++) = n * positions[i][j][1];
            *(d++) = n * positions[i][j][2];
            *(d++) = normals[i][0];
            *(d++) = normals[i][1];
            *(d++) = normals[i][2];
            *(d++) = du + (uvs[i][j][0] ? b : a);
            *(d++) = dv + (uvs[i][j][1] ? b : a);
            *(d++) = ao;
            *(d++) = light;
        }
    }
    float ma[16];
    float mb[16];
    mat_identity(ma);
    mat_rotate(mb, 0, 1, 0, RADIANS(rotation));
    mat_multiply(ma, mb, ma);
    mat_apply(data, ma, 24, 3, 10);
    mat_translate(mb, px, py, pz);
    mat_multiply(ma, mb, ma);
    mat_apply(data, ma, 24, 0, 10);
}


void make_cube_faces(
        float *data, float ao[6][4], float light[6][4],
        int left, int right, int top, int bottom, int front, int back,
        int wleft, int wright, int wtop, int wbottom, int wfront, int wback,
        float x, float y, float z, float n)
{
    static const float positions[6][4][3] = {
            {{-1, -1, -1}, {-1, -1, +1}, {-1, +1, -1}, {-1, +1, +1}},
            {{+1, -1, -1}, {+1, -1, +1}, {+1, +1, -1}, {+1, +1, +1}},
            {{-1, +1, -1}, {-1, +1, +1}, {+1, +1, -1}, {+1, +1, +1}},
            {{-1, -1, -1}, {-1, -1, +1}, {+1, -1, -1}, {+1, -1, +1}},
            {{-1, -1, -1}, {-1, +1, -1}, {+1, -1, -1}, {+1, +1, -1}},
            {{-1, -1, +1}, {-1, +1, +1}, {+1, -1, +1}, {+1, +1, +1}}
    };
    static const float normals[6][3] = {
            {-1, 0, 0},
            {+1, 0, 0},
            {0, +1, 0},
            {0, -1, 0},
            {0, 0, -1},
            {0, 0, +1}
    };
    static const float uvs[6][4][2] = {
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
            {{1, 0}, {0, 0}, {1, 1}, {0, 1}},
            {{0, 1}, {0, 0}, {1, 1}, {1, 0}},
            {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
            {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
            {{1, 0}, {1, 1}, {0, 0}, {0, 1}}
    };
    static const float indices[6][6] = {
            {0, 3, 2, 0, 1, 3},
            {0, 3, 1, 0, 2, 3},
            {0, 3, 2, 0, 1, 3},
            {0, 3, 1, 0, 2, 3},
            {0, 3, 2, 0, 1, 3},
            {0, 3, 1, 0, 2, 3}
    };
    static const float flipped[6][6] = {
            {0, 1, 2, 1, 3, 2},
            {0, 2, 1, 2, 3, 1},
            {0, 1, 2, 1, 3, 2},
            {0, 2, 1, 2, 3, 1},
            {0, 1, 2, 1, 3, 2},
            {0, 2, 1, 2, 3, 1}
    };
    float *d = data;
    float s = 0.0625;
    float a = 0 + 1 / 2048.0;
    float b = s - 1 / 2048.0;
    int faces[6] = {left, right, top, bottom, front, back};
    int tiles[6] = {wleft, wright, wtop, wbottom, wfront, wback};
    for (int i = 0; i < 6; i++) {
        if (faces[i] == 0) {
            continue;
        }
        float du = (tiles[i] % 16) * s;
        float dv = (tiles[i] / 16) * s;
        int flip = ao[i][0] + ao[i][3] > ao[i][1] + ao[i][2];
        for (int v = 0; v < 6; v++) {
            int j = flip ? flipped[i][v] : indices[i][v];
            *(d++) = x + n * positions[i][j][0];
            *(d++) = y + n * positions[i][j][1];
            *(d++) = z + n * positions[i][j][2];
            *(d++) = normals[i][0];
            *(d++) = normals[i][1];
            *(d++) = normals[i][2];
            *(d++) = du + (uvs[i][j][0] ? b : a);
            *(d++) = dv + (uvs[i][j][1] ? b : a);
            *(d++) = ao[i][j];
            *(d++) = light[i][j];
        }
    }
}

void make_cube(
        float *data, float ao[6][4], float light[6][4],
        int left, int right, int top, int bottom, int front, int back,
        float x, float y, float z, float n, int w)
{
    int wleft = blocks[w][0];
    int wright = blocks[w][1];
    int wtop = blocks[w][2];
    int wbottom = blocks[w][3];
    int wfront = blocks[w][4];
    int wback = blocks[w][5];
    make_cube_faces(
            data, ao, light,
            left, right, top, bottom, front, back,
            wleft, wright, wtop, wbottom, wfront, wback,
            x, y, z, n);
}
