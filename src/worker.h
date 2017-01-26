//
// Created by nathan on 1/21/17.
//

#ifndef CRAFT_WORKER_H
#define CRAFT_WORKER_H

#include <GLFW/glfw3.h>
#include "map.h"
extern "C" {
    #include "tinycthread.h"
}
#include <memory>

class WorkerItem {
public:
    int p;
    int q;
    int load;
    Map *block_maps[3][3];
    Map *light_maps[3][3];
    int miny;
    int maxy;
    int faces;
    GLfloat *data;
};

typedef std::shared_ptr<WorkerItem> WorkerItemPtr;

class Worker {
public:
    int index;
    int state;
    thrd_t thrd;
    mtx_t mtx;
    cnd_t cnd;
    WorkerItemPtr item;
};

#endif //CRAFT_WORKER_H
