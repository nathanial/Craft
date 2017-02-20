//
// Created by nathan on 1/18/17.
//

#ifndef CRAFT_DRAW_H
#define CRAFT_DRAW_H


void draw_triangles_3d_ao(Attrib *attrib, GLuint buffer, int count);
void draw_triangles_3d(Attrib *attrib, GLuint buffer, int count);
void draw_triangles_2d(Attrib *attrib, GLuint buffer, int count);
void draw_lines(Attrib *attrib, GLuint buffer, int components, int count);
void draw_item(Attrib *attrib, GLuint buffer, int count);
void draw_text(Attrib *attrib, GLuint buffer, int length);
void draw_cube(Attrib *attrib, GLuint buffer);
void draw_plant(Attrib *attrib, GLuint buffer);
void draw_player(Attrib *attrib, Player *player);

#endif //CRAFT_DRAW_H
