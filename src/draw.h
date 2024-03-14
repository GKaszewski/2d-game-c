#ifndef DRAW_H_H
#define DRAW_H_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint load_texture(const char *path);
char* read_shader_source(const char *file_path);
GLuint load_shaders(const char *vertex_file_path, const char *fragment_file_path);

typedef struct {
    float position[2];
    float tex_coords[2];
    float color[4]; // RGBA
} Vertex;

typedef struct {
    float position[2];
    float size[2];
    float color[4];
    float tex_coords[4];
    float rotation;
} Sprite;

typedef struct {
    GLuint vao, vbo, ebo;
    Vertex* verticies;
    GLuint* indicies;
    GLuint texture;
    GLuint shader;
    int sprite_count;
} SpriteBatch;

extern const int MAX_SPRITES;
extern const short VERTICIES_PER_SPRITE;
extern const short INDICIES_PER_SPRITE;

void init_sprite_batch(SpriteBatch* batch, const char* texture_path);
void add_sprite(SpriteBatch* batch, Sprite* sprite);
void update_sprite(SpriteBatch* batch, int index, Sprite* sprite);
void draw_sprite_batch(SpriteBatch* batch);
void set_sprite_tex_coords(Sprite* sprite, float atlas_width, float atlas_height, float x, float y, float width, float height);

#endif