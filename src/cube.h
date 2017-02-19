#ifndef _cube_h_
#define _cube_h_

#include <vector>

std::vector<float> make_cube_faces(float ao[6][4], float light[6][4],
    int left, int right, int top, int bottom, int front, int back,
    int wleft, int wright, int wtop, int wbottom, int wfront, int wback,
    float x, float y, float z, float n);

std::vector<float> make_cube(float ao[6][4], float light[6][4],
    int left, int right, int top, int bottom, int front, int back,
    float x, float y, float z, float n, int w);

std::vector<float> make_plant(float ao, float light,
    float px, float py, float pz, float n, int w, float rotation);

std::vector<float> make_player(float x, float y, float z, float rx, float ry);

void make_cube_wireframe(
    std::vector<float> &data, float x, float y, float z, float n);

void make_character(
    float *data,
    float x, float y, float n, float m, char c);

void make_character_3d(
    float *data, float x, float y, float z, float n, int face, char c);

void make_sphere(float *data, float r, int detail);

#endif
