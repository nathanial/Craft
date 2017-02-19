#ifndef _matrix_h_
#define _matrix_h_

#include <vector>
#include <armadillo>

void normalize(float *x, float *y, float *z);
void mat_identity(float *matrix);
void mat_translate(float *matrix, float dx, float dy, float dz);

arma::mat mat_translate(float dx, float dy, float dz);
void mat_rotate(float *matrix, float x, float y, float z, float angle);
arma::mat mat_rotate(float x, float y, float z, float t);

void mat_multiply(float *matrix, float *a, float *b);

void mat_apply(std::vector<float> &data, float *matrix, int count, int offset, int stride);
void mat_apply(std::vector<float>& d, arma::mat &ma, int count, int offset, int stride);

void frustum_planes(float planes[6][4], int radius, float *matrix);
arma::mat mat_frustum(float left, float right, float bottom, float top, float znear, float zfar);
arma::mat mat_perspective(float fov, float aspect, float near, float far);
arma::mat mat_ortho(float left, float right, float bottom, float top, float near, float far);
void set_matrix_2d(float *matrix, int width, int height);


arma::mat set_matrix_3d( int width, int height,
        float x, float y, float z, float rx, float ry,
        float fov, int ortho, int radius);
void set_matrix_3d(
    float *matrix, int width, int height,
    float x, float y, float z, float rx, float ry,
    float fov, int ortho, int radius);
void set_matrix_item(float *matrix, int width, int height, int scale);

#endif
