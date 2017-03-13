#include <GL/glew.h>
#include "model.h"
#include "util.h"
#include "item.h"

extern Model *g;


int collide(int height, float *x, float *y, float *z) {
    int result = 0;
    int p = chunked(*x);
    int q = chunked(*z);
    auto chunk = g->find_chunk(p, q);
    if (!chunk) {
        return result;
    }
    int nx = roundf(*x);
    int ny = roundf(*y);
    int nz = roundf(*z);
    float px = *x - nx;
    float py = *y - ny;
    float pz = *z - nz;
    float pad = 0.25;
    for (int dy = 0; dy < height; dy++) {
        if (px < -pad && is_obstacle(g->get_block(nx - 1, ny - dy, nz))) {
            *x = nx - pad;
        }
        if (px > pad && is_obstacle(g->get_block(nx + 1, ny - dy, nz))) {
            *x = nx + pad;
        }
        if (py < -pad && is_obstacle(g->get_block(nx, ny - dy - 1, nz))) {
            *y = ny - pad;
            result = 1;
        }
        if (py > pad && is_obstacle(g->get_block(nx, ny - dy + 1, nz))) {
            *y = ny + pad;
            result = 1;
        }
        if (pz < -pad && is_obstacle(g->get_block(nx, ny - dy, nz - 1))) {
            *z = nz - pad;
        }
        if (pz > pad && is_obstacle(g->get_block(nx, ny - dy, nz + 1))) {
            *z = nz + pad;
        }
    }
    return result;
}

void handle_movement(double dt) {
    static float dy = 0;
    State *s = &g->player.state;
    int sz = 0;
    int sx = 0;
    float m = dt * 1.0;
    g->ortho = glfwGetKey(g->window, CRAFT_KEY_ORTHO) ? 64 : 0;
    g->fov = glfwGetKey(g->window, CRAFT_KEY_ZOOM) ? 15 : 65;
    if (glfwGetKey(g->window, CRAFT_KEY_FORWARD)) sz--;
    if (glfwGetKey(g->window, CRAFT_KEY_BACKWARD)) sz++;
    if (glfwGetKey(g->window, CRAFT_KEY_LEFT)) sx--;
    if (glfwGetKey(g->window, CRAFT_KEY_RIGHT)) sx++;
    if (glfwGetKey(g->window, GLFW_KEY_LEFT)) s->rx -= m;
    if (glfwGetKey(g->window, GLFW_KEY_RIGHT)) s->rx += m;
    if (glfwGetKey(g->window, GLFW_KEY_UP)) s->ry += m;
    if (glfwGetKey(g->window, GLFW_KEY_DOWN)) s->ry -= m;
    float vx, vy, vz;
    get_motion_vector(g->flying, sz, sx, s->rx, s->ry, &vx, &vy, &vz);
    if (glfwGetKey(g->window, CRAFT_KEY_JUMP)) {
        if (g->flying) {
            vy = 1;
        }
        else if (dy == 0) {
            dy = 8;
        }
    }
    float speed = g->flying ? 20 : 5;
    int estimate = roundf(sqrtf(
            powf(vx * speed, 2) +
            powf(vy * speed + ABS(dy) * 2, 2) +
            powf(vz * speed, 2)) * dt * 8);
    int step = MAX(8, estimate);
    float ut = dt / step;
    vx = vx * ut * speed;
    vy = vy * ut * speed;
    vz = vz * ut * speed;
    for (int i = 0; i < step; i++) {
        if (g->flying) {
            dy = 0;
        }
        else {
            dy -= ut * 25;
            dy = MAX(dy, -250);
        }
        s->x += vx;
        s->y += vy + dy * ut;
        s->z += vz;
        if (collide(2, &s->x, &s->y, &s->z)) {
            dy = 0;
        }
    }
    if (s->y < 0) {
        s->y = highest_block(s->x, s->z) + 2;
    }
}