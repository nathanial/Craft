//
// Created by nathan on 2/1/17.
//

#ifndef CRAFT_TASK_H
#define CRAFT_TASK_H

#include <future>

template<typename T>
class Task {
public:
    virtual std::future<T> run() = 0;
};

#endif // CRAFT_TASK_H
