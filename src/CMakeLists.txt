FILE(GLOB SOURCE_FILES *.cpp)

#add_definitions(-fsanitize=address -fuse-ld=gold -fno-omit-frame-pointer)
add_definitions(-O2)

include_directories(../deps/glew/include)
include_directories(../deps/glfw/include)
include_directories(../deps/lodepng)
include_directories(../deps/noise)
include_directories(../deps/sqlite)
include_directories(../deps/tinycthread)


find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(
        craft
        ${SOURCE_FILES}
        ../deps/glew/src/glew.c
        ../deps/lodepng/lodepng.cpp
        ../deps/noise/noise.c
        ../deps/sqlite/sqlite3.c
        chunk.h chunk.cpp model.h
        draw.cpp draw.h
        biomes/rolling_planes.cpp biomes/rolling_planes.h
        biomes/biome.h biomes/mountains.h biomes/mountains.cpp
        height_map.h
        biomes/flatland.h
        biomes/flatland.cpp)

target_link_libraries(craft dl glfw
        ${GLFW_LIBRARIES} ${CURL_LIBRARIES})