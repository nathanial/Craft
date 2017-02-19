#include <math.h>
#include "config.h"
#include "matrix.h"
#include "util.h"
#include <armadillo>

void normalize(float *x, float *y, float *z) {
    float d = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    *x /= d; *y /= d; *z /= d;
}

void copy_matrix(float *dst, const arma::mat& src){
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			dst[i * 4 + j] = src(i,j);
		}
	}
}

arma::mat copy_from_array(float *src){
	arma::mat dst(4,4);
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			dst(i,j) = src[i * 4 + j];
		}
	}
	return dst;
}

arma::mat mat_translate(float dx, float dy, float dz) {
    arma::mat translate(4,4, arma::fill::zeros);
    translate.eye();
    translate(3,0) = dx;
    translate(3,1) = dy;
    translate(3,2) = dz;
    return translate;
}

arma::mat mat_rotate(float x, float y, float z, float t) {
    float ux = x;
    float uy = y;
    float uz = z;
    normalize(&ux, &uy, &uz);
    return arma::mat {
            { cos(t)+(ux*ux)*(1-cos(t)), (ux*uy)*(1-cos(t))-uz*sin(t), ux*uz*(1-cos(t)) + uy*sin(t), 0 },
            { uy*uz*(1-cos(t))+uz*sin(t), cos(t)+(uy*uy)*(1-cos(t)), uy*uz*(1-cos(t)) - ux*sin(t), 0},
            { uz*ux*(1-cos(t))-uy*sin(t), uz*uy*(1-cos(t)) + ux*sin(t), cos(t) + uz*uz*(1 - cos(t)), 0 },
            { 0, 0, 0, 1}
    };
}

void mat_apply(std::vector<float>& d, arma::mat &ma, int count, int offset, int stride) {
	arma::mat vec = {0,0,0,1};
	for (int i = 0; i < count; i++) {
        int cursor = offset + stride * i;
		vec(0,0) = d.at(cursor++);
		vec(0,1) = d.at(cursor++);
		vec(0,2) = d.at(cursor++);

		vec = vec * ma;

		cursor = offset + stride * i;
		d.at(cursor++) = vec(0,0);
		d.at(cursor++) = vec(0,1);
		d.at(cursor++) = vec(0,2);
	}
}

arma::mat frustum_planes(int radius, float *matrix) {
    arma::mat planes(6,4);
    float znear = 0.125;
    float zfar = radius * 32 + 64;
    float *m = matrix;
    planes(0,0) = m[3] + m[0];
    planes(0,1) = m[7] + m[4];
    planes(0,2) = m[11] + m[8];
    planes(0,3) = m[15] + m[12];
    planes(1,0) = m[3] - m[0];
    planes(1,1) = m[7] - m[4];
    planes(1,2) = m[11] - m[8];
    planes(1,3) = m[15] - m[12];
    planes(2,0) = m[3] + m[1];
    planes(2,1) = m[7] + m[5];
    planes(2,2) = m[11] + m[9];
    planes(2,3) = m[15] + m[13];
    planes(3,0) = m[3] - m[1];
    planes(3,1) = m[7] - m[5];
    planes(3,2) = m[11] - m[9];
    planes(3,3) = m[15] - m[13];
    planes(4,0) = znear * m[3] + m[2];
    planes(4,1) = znear * m[7] + m[6];
    planes(4,2) = znear * m[11] + m[10];
    planes(4,3) = znear * m[15] + m[14];
    planes(5,0) = zfar * m[3] - m[2];
    planes(5,1) = zfar * m[7] - m[6];
    planes(5,2) = zfar * m[11] - m[10];
    planes(5,3) = zfar * m[15] - m[14];
    return planes;
}

arma::mat mat_frustum(float left, float right, float bottom, float top, float znear, float zfar)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0 * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    return arma::mat {
        {temp / temp2, 0.0, 0.0, 0.0},
        {0.0, temp / temp3, 0.0, 0.0},
        {(right + left) / temp2, (top + bottom) / temp3, (-zfar - znear) / temp4, -1.0},
        {0.0, 0.0, (-temp * zfar) / temp4, 0.0}
    };
}


arma::mat mat_perspective(float fov, float aspect, float znear, float zfar) {
    float ymax, xmax;
    ymax = znear * tanf(fov * PI / 360.0);
    xmax = ymax * aspect;
    return mat_frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

arma::mat mat_ortho(float left, float right, float bottom, float top, float near, float far) {
    return arma::mat {
        {2 / (right - left), 0, 0, 0},
        {0, 2 / (top - bottom), 0, 0},
        {0, 0, -2 / (far - near), 0},
        {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1}
    };
}

void set_matrix_2d(float *matrix, int width, int height) {
    auto m = mat_ortho(0, width, 0, height, -1, 1);
    copy_matrix(matrix, m);
}

arma::mat set_matrix_3d(int width, int height,
        float x, float y, float z, float rx, float ry,
        float fov, int ortho, int radius){

    arma::mat a(4,4);
    arma::mat b(4,4);
    float aspect = (float)width / height;
    float znear = 0.125;
    float zfar = radius * 32 + 64;
    a.eye();
    b = mat_translate(-x, -y, -z);
    a = a * b;
    b = mat_rotate(cosf(rx), 0, sinf(rx), ry);
    a = a * b;
    b = mat_rotate(0, 1, 0, -rx);
    a = a * b;
    if (ortho) {
        int size = ortho;
        b = mat_ortho(-size * aspect, size * aspect, -size, size, -zfar, zfar);
    }
    else {
        b = mat_perspective(fov, aspect, znear, zfar);
    }
    a = a * b;
    arma::mat m(4,4);
    m.eye();
    m = m* a;
    return m;
}

arma::mat set_matrix_item(int width, int height, int scale) {
    arma::mat a(4,4);
    arma::mat b(4,4);
    float aspect = (float)width / height;
    float size = 64 * scale;
    float box = height / size / 2;
    float xoffset = 1 - size / width * 2;
    float yoffset = 1 - size / height * 2;
    a.eye();
    b = mat_rotate(0, 1, 0, -PI / 4);
    a = a * b;
    b = mat_rotate(1, 0, 0, -PI / 10);
    a = a * b;
    b = mat_ortho(-box * aspect, box * aspect, -box, box, -1, 1);
    a = a * b;
    b = mat_translate(-xoffset, -yoffset, 0);
    a = a * b;
    arma::mat m(4,4);
    m.eye();
    m = m * a;
    return m;
}
