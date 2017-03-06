//
// Created by Nathanial Hartman on 3/6/17.
//

#include "WorldActor.h"
using namespace vgk::game::world;

void WorldActor::start() {
    running = true;
    thread = std::thread([&]{
        while(true){
            std::unique_lock<std::mutex> lock(request_mtx);
            if(!running){
                return;
            }
            request_recv.wait(lock);

            auto &request = requests.front();
            this->reply(*request);
            requests.pop_front();
        }
    });
}

void WorldActor::stop(){
    std::lock_guard<std::mutex> lock(request_mtx);
    this->running = false;
}

std::future<Response> WorldActor::request(const Request& request) {
    std::unique_lock<std::mutex> lock(request_mtx);
    auto req_res = std::make_shared<WorldRequestResponse>();
    req_res->request = request;
    req_res->response = std::promise<Response>();
    this->requests.push_back(req_res);
    request_recv.notify_all();
    return req_res->response.get_future();
}

void WorldActor::reply(WorldRequestResponse &req_res){
    auto &request = req_res.request;
    switch(request.type){
        case SET_BLOCK: {
            auto data = request.data.set_block_data;
            this->set_block(data.x, data.y, data.z, data.value);
            Response response;
            response.status = SUCCESS;
            req_res.response.set_value(response);
        }
        break;

        case CREATE_CHUNK:
        break;

        default:
        throw "Unknown request type";
    }
}

void WorldActor::set_block(int x, int y, int z, char value) {
    
}