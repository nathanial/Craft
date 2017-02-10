//
// Created by nathan on 2/10/17.
//

#ifndef CRAFT_CHUNK_MESH_H
#define CRAFT_CHUNK_MESH_H

#include <GL/glew.h>
#include "chunk.h"

class ChunkMesh {
private:
    int _faces;
    int _miny;
    int _maxy;
    GLfloat *_vertices;
    bool _dirty;
    GLuint _buffer = 0;

    void populate_opaque_array(const Neighborhood &neighborhood, BigBlockMap *opaque, HeightMap<48> *highest, int ox, int oy, int oz) const;
    void populate_light_array(const Neighborhood &neighborhood, BigBlockMap *opaque, BigBlockMap *light, int ox, int oy, int oz) const;

public:
    int p,q;
    ChunkMesh(const ChunkPtr chunk, const Neighborhood& neighborhood);
    ~ChunkMesh();
    int draw(Attrib *attrib);
    void generate_buffer();
    void set_dirty(bool dirty);
    bool dirty() const;
    bool is_ready_to_draw() const;
    bool has_buffer() const;

    int miny() const;
    int maxy() const;


    int distance(int p, int q);
};

typedef std::shared_ptr<ChunkMesh> ChunkMeshPtr;

#endif //CRAFT_CHUNK_MESH_H
