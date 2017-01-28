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
    int miny;
    int maxy;
    int faces;
    GLfloat *data;
    ChunkPtr neighborhood[3][3];
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
