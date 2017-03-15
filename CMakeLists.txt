cmake_minimum_required(VERSION 2.8)

project(craft)

set(CMAKE_BUILD_TYPE DEBUG)

FILE(GLOB SOURCE_FILES src/*.cpp)

add_executable(
    craft
    ${SOURCE_FILES}
    deps/glew/src/glew.c
        deps/lodepng/lodepng.cpp
    deps/noise/noise.c
        src/chunk/chunk.h src/chunk/chunk.cpp src/model.h src/draw.cpp src/draw.h src/actors/Actors.cpp src/actors/Actors.h src/biomes/rolling_planes.cpp src/biomes/rolling_planes.h src/biomes/biome.h src/biomes/mountains.h src/biomes/mountains.cpp src/height_map.h src/workers/tasks/generate_chunk_task.h src/workers/tasks/task.h src/workers/tasks/generate_chunk_task.cpp src/biomes/flatland.cpp src/biomes/flatland.h src/terrain_features/trees/OakTree.cpp src/terrain_features/trees/OakTree.h src/terrain_features/TerrainFeature.cpp src/terrain_features/TerrainFeature.h src/chunk/ChunkMesh.cpp src/chunk/ChunkMesh.h src/chunk/TransientChunk.cpp src/chunk/TransientChunk.h src/chunk/TransientChunkMesh.cpp src/chunk/TransientChunkMesh.h src/actors/World.cpp src/actors/World.h)

set(CMAKE_CXX_FLAGS "-std=c++14 -fpermissive -Wno-narrowing")
set(CMAKE_C_FLAGS)

add_subdirectory(deps/glfw)
include_directories(deps/glew/include)
include_directories(deps/glfw/include)
include_directories(deps/lodepng)
include_directories(deps/noise)
include_directories(deps/sqlite)
include_directories(deps/tinycthread)
include_directories(/usr/local/include)

if(MINGW)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}
        "C:/Program Files/CURL/lib" "C:/Program Files (x86)/CURL/lib")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH}
        "C:/Program Files/CURL/include" "C:/Program Files (x86)/CURL/include")
endif()

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

if(APPLE)
    target_link_libraries(craft glfw -larmadillo -lcaf_core
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})
endif()

if(UNIX)
    target_link_libraries(craft dl glfw -larmadillo
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})
endif()

if(MINGW)
    target_link_libraries(craft ws2_32.lib glfw
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})
endif()
