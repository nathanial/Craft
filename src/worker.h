//
// Created by nathan on 1/21/17.
//

#ifndef CRAFT_WORKER_H
#define CRAFT_WORKER_H

#include <GLFW/glfw3.h>
#include "block_map.h"
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class WorkerItem {
public:
    int p;
    int q;
    int load;
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
    std::thread thrd;
    std::mutex mtx;
    std::condition_variable cnd;
    WorkerItemPtr item;
};

typedef std::shared_ptr<Worker> WorkerPtr;

#endif //CRAFT_WORKER_H
