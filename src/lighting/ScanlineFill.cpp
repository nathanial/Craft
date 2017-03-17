//
// Created by Nathanial Hartman on 3/17/17.
//

#include "ScanlineFill.h"
#include "../item.h"
#include <deque>


enum class SourceDirection {
    None,
    ZMinus,
    ZPlus,
    YMinus,
    YPlus
};

void light_fill_scanline(const BigBlockMap &opaque, BigBlockMap &light, int ox, int oy ,int oz, int ow);

void scanline_iterate(BigBlockMap &light, const BigBlockMap &opaque, std::deque<std::tuple<int, int, int, int, SourceDirection>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, int direction);

bool is_sunlight(int y, int w){
    return y == CHUNK_HEIGHT - 2 && w == 0;
}

std::unique_ptr<BigBlockMap> ScanlineFill::light(int p, int q, const BigBlockMap &opaque, const ChunkNeighbors& neighbors) {
    int ox = p * CHUNK_SIZE - CHUNK_SIZE;
    int oy = -1;
    int oz = q * CHUNK_SIZE - CHUNK_SIZE;
    auto light = std::make_unique<BigBlockMap>();

    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            auto chunk = neighbors.at(std::make_tuple(p - (a - 1), q - (b - 1)));
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
                            if(ey == CHUNK_HEIGHT){
                                continue;
                            }

                            int lx = ex - ox;
                            int ly = ey - oy;
                            int lz = ez - oz;

                            if (is_light(ew)) {
                                light_fill_scanline(opaque, *light, lx, ly, lz, 15);
                            }
//                            else if(is_sunlight(ey, ew)) {
//                                light_fill_scanline(opaque, *light, lx, ly, lz, 15);
//                            }
                        }
                    }
                }
            }
        }
    }
    return light;
}



void light_fill_scanline(const BigBlockMap &opaque, BigBlockMap &light, int ox, int oy ,int oz, int ow)
{
    std::deque<std::tuple<int,int,int,int, SourceDirection >> frontier;
    frontier.push_back(std::make_tuple(ox,oy,oz,ow, SourceDirection::None));
    int check_count = 0;
    std::cout << "Light Fill Scanline " << ox << "," << oy << "," << oz << "," << ow << std::endl;
    while(!frontier.empty()){
        auto &next = frontier.front();
        int x = std::get<0>(next);
        int y = std::get<1>(next);
        int z = std::get<2>(next);
        int w = std::get<3>(next);
        frontier.pop_front();

        //std::cout << "Check " << x << "," << y << "," << z << " | " << check_count++ << std::endl;

        if(w == 0){
            continue;
        }
        if(y >= CHUNK_HEIGHT) {
            continue;
        }
        if(opaque.get(x,y,z)){
            continue;
        }
        if(light.get(x,y,z) >= w){
            continue;
        }

        scanline_iterate(light, opaque, frontier, x, y, z, w, x, w, 1);
        scanline_iterate(light, opaque, frontier, x, y, z, w, x-1, w, -1);
    }
}

void scanline_iterate(BigBlockMap &light, const BigBlockMap &opaque,
                      std::deque<std::tuple<int, int, int, int, SourceDirection>> &frontier,
                      int x, int y, int z, int w,
                      int cursorX, int cursorW, int direction) {

    auto canLight = [&](int x, int y, int z, int w){
        return light.get(x, y, z) < w && !opaque.get(x, y, z);
    };

    bool spanZMinus = false, spanZPlus = false, spanYMinus = false, spanYPlus = false;
    while(cursorX < light.width() && cursorX >= 0 && canLight(cursorX, y, z, w - ABS(x - cursorX))){
        cursorW = w - ABS(x - cursorX);
        light.set(cursorX, y, z, cursorW);
        if(!spanZMinus && z > 0 && canLight(cursorX, y, z-1, cursorW-1)) {
            frontier.push_back(std::make_tuple(cursorX, y, z - 1, cursorW - 1, SourceDirection::ZMinus));
            spanZMinus = true;
        } else if(spanZMinus && z > 0 && !canLight(cursorX, y, z - 1, cursorW-1)){
            spanZMinus = false;
        }
        if(!spanZPlus && z < light.width() - 1 && canLight(cursorX, y, z+1, cursorW - 1)){
            frontier.push_back(std::make_tuple(cursorX, y, z + 1, cursorW - 1, SourceDirection::ZPlus));
            spanZPlus = true;
        } else if(spanZPlus && z < light.width() - 1 && !canLight(cursorX, y, z + 1, cursorW - 1)){
            spanZPlus = false;
        }
        if(!spanYMinus && y > 0 && canLight(cursorX, y-1,z, cursorW-1)){
            frontier.push_back(std::make_tuple(cursorX, y-1, z, cursorW-1, SourceDirection::YMinus));
            spanYMinus = true;
        } else if(spanYMinus && y > 0 && !canLight(cursorX, y-1,z, cursorW- 1)){
            spanYMinus = false;
        }
        if(!spanYPlus && y < light.height() - 1 && canLight(cursorX, y+1,z, cursorW-1)){
            frontier.push_back(std::make_tuple(cursorX, y+1,z, cursorW-1, SourceDirection::YPlus));
            spanYPlus = true;
        } else if(spanYPlus && y < light.height() - 1 && !canLight(cursorX, y+1, z, cursorW-1)){
            spanYPlus = false;
        }

        cursorX += direction;

    }
}
