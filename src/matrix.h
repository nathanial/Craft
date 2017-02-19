#ifndef _matrix_h_
#define _matrix_h_

#include <vector>
#include <armadillo>

void copy_matrix(float *dst, const arma::mat& src);

void normalize(float *x, float *y, float *z);
arma::mat mat_translate(float dx, float dy, float dz);

arma::mat mat_rotate(float x, float y, float z, float t);

void mat_apply(std::vector<float>& d, arma::mat &ma, int count, int offset, int stride);

arma::mat frustum_planes(int radius, float *matrix);
arma::mat mat_frustum(float left, float right, float bottom, float top, float znear, float zfar);
arma::mat mat_perspective(float fov, float aspect, float near, float far);
arma::mat mat_ortho(float left, float right, float bottom, float top, float near, float far);
void set_matrix_2d(float *matrix, int width, int height);


arma::mat set_matrix_3d( int width, int height,
        float x, float y, float z, float rx, float ry,
        float fov, int ortho, int radius);
arma::mat set_matrix_3d(int width, int height,
                        float x, float y, float z, float rx, float ry,
                        float fov, int ortho, int radius);
arma::mat set_matrix_item(int width, int height, int scale);

#endif
