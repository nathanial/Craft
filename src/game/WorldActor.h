//
// Created by Nathanial Hartman on 3/6/17.
//

#ifndef CRAFT_WORLDACTOR_H
#define CRAFT_WORLDACTOR_H


#include <deque>
#include <future>
#include "../chunk/chunk.h"

namespace vgk {
namespace game {
namespace world {
    enum RequestType {
        SET_BLOCK,
        CREATE_CHUNK
    };

    struct SetBlockData {
        int x, y, z;
        char value;
    };

    union RequestData {
        SetBlockData set_block_data;
    };

    struct Request {
        RequestType type;
        RequestData data;
    };

    enum ResponseStatus {
        SUCCESS,
        FAILURE
    };

    struct Response {
        ResponseStatus status;
    };

    struct WorldRequestResponse {
        Request request;
        std::promise<Response> response;
    };


    typedef std::tuple<std::shared_ptr<Chunk>, std::shared_ptr<ChunkMesh>> ChunkAndMesh;

    class WorldActor {
    private:
        std::unordered_map<ChunkPosition, ChunkPtr, ChunkPositionHash> chunks;
        std::unordered_map<ChunkPosition, std::shared_ptr<ChunkMesh>, ChunkPositionHash> meshes;
        std::deque<std::shared_ptr<WorldRequestResponse>> requests;
        std::mutex request_mtx;
        std::thread thread;
        std::condition_variable request_recv;
        bool running = false;

        void reply(WorldRequestResponse &req_res);

        void set_block(int x, int y, int z, char value);

    public:
        void start();

        void stop();

        std::future<Response> request(const Request &request);

    };
}
}
}


#endif //CRAFT_WORLDACTOR_H
