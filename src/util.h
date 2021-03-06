#ifndef _util_h_
#define _util_h_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "config.h"
#include <vector>

#define PI 3.14159265359
#define DEGREES(radians) ((radians) * 180 / PI)
#define RADIANS(degrees) ((degrees) * PI / 180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) (((x) > 0) - ((x) < 0))

#if DEBUG
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

typedef struct {
    unsigned int fps;
    unsigned int frames;
    double since;
} FPS;

void update_fps(FPS *fps);

GLuint gen_buffer(GLsizei size, GLfloat *data);
GLuint gen_buffer(const std::vector<GLfloat> &data);
void del_buffer(GLuint buffer);
GLuint gen_faces(int components, int faces, const std::vector<GLfloat> &data);
GLuint make_shader(GLenum type, const char *source);
GLuint load_shader(GLenum type, const char *path);
GLuint make_program(GLuint shader1, GLuint shader2);
GLuint load_program(const char *path1, const char *path2);
void load_png_texture(const char *file_name);
void get_sight_vector(float rx, float ry, float *vx, float *vy, float *vz);
void get_motion_vector(int flying, int sz, int sx, float rx, float ry, float *vx, float *vy, float *vz);
int chunked(float x);
void add_all(std::vector<float> &dst, const std::vector<float> &src);
#endif
