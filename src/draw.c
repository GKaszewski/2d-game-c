#include "draw.h"
#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

const int MAX_SPRITES = 1000;
const short VERTICIES_PER_SPRITE = 4;
const short INDICIES_PER_SPRITE = 6;

GLuint load_texture(const char *path) {
  int width, height, channels;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
  if (!data) {
    printf("Failed to load texture: %s\n", path);
    return 0;
  }

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format;
  if (channels == 1)
    format = GL_RED;
  else if (channels == 3)
    format = GL_RGB;
  else
    format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  return texture_id;
}

char *read_shader_source(const char *file_path) {
  FILE *shader_file = fopen(file_path, "r");
  if (shader_file == NULL) {
    printf("Could not open %s.\n", file_path);
    return NULL;
  }

  fseek(shader_file, 0, SEEK_END);
  long file_size = ftell(shader_file);
  rewind(shader_file);

  char *buffer = (char *)malloc((file_size + 1) * sizeof(char));
  if (buffer == NULL) {
    printf("Failed to allocate memory for shader source.\n");
    return NULL;
  }

  size_t bytes_read = fread(buffer, sizeof(char), file_size, shader_file);
  if (bytes_read != file_size) {
    printf("Error reading %s.\n", file_path);
    free(buffer);
    return NULL;
  }

  buffer[bytes_read] = '\0'; // Null-terminate the string

  fclose(shader_file);
  return buffer;
}

GLuint load_shaders(const char *vertex_file_path,
                    const char *fragment_file_path) {
  char *vertex_source = read_shader_source(vertex_file_path);
  char *fragment_source = read_shader_source(fragment_file_path);
  if (vertex_source == NULL || fragment_source == NULL) {
    return 0;
  }

  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  GLuint result = GL_FALSE;
  int log_length;

  printf("Compiling vertex shader: %s\n", vertex_file_path);
  glShaderSource(vertex_shader_id, 1, (const GLchar **)&vertex_source, NULL);
  glCompileShader(vertex_shader_id);

  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    char *log = malloc(log_length);
    glGetShaderInfoLog(vertex_shader_id, log_length, NULL, log);
    printf("%s\n", log);
    free(log);
  }

  printf("Compiling fragment shader: %s\n", fragment_file_path);
  glShaderSource(fragment_shader_id, 1, (const GLchar **)&fragment_source,
                 NULL);
  glCompileShader(fragment_shader_id);

  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    char *log = malloc(log_length);
    glGetShaderInfoLog(fragment_shader_id, log_length, NULL, log);
    printf("%s\n", log);
    free(log);
  }

  printf("Linking program\n");
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    char *log = malloc(log_length);
    glGetProgramInfoLog(program_id, log_length, NULL, log);
    printf("%s\n", log);
    free(log);
  }

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  free(vertex_source);
  free(fragment_source);

  return program_id;
}

void init_sprite_batch(SpriteBatch *batch, const char *texture_path) {
  batch->sprite_count = 0;

  batch->verticies =
      malloc(sizeof(Vertex) * MAX_SPRITES * VERTICIES_PER_SPRITE);
  batch->indicies = malloc(sizeof(GLuint) * MAX_SPRITES * INDICIES_PER_SPRITE);

  int offset = 0;
  for (int i = 0; i < MAX_SPRITES * INDICIES_PER_SPRITE;
       i += INDICIES_PER_SPRITE, offset += VERTICIES_PER_SPRITE) {
    batch->indicies[i] = offset;
    batch->indicies[i + 1] = offset + 1;
    batch->indicies[i + 2] = offset + 2;
    batch->indicies[i + 3] = offset + 2;
    batch->indicies[i + 4] = offset + 3;
    batch->indicies[i + 5] = offset;
  }

  batch->texture = load_texture(texture_path);

  glGenVertexArrays(1, &batch->vao);
  glGenBuffers(1, &batch->vbo);
  glGenBuffers(1, &batch->ebo);

  glBindVertexArray(batch->vao);

  glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vertex) * MAX_SPRITES * VERTICIES_PER_SPRITE, NULL,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLuint) * MAX_SPRITES * INDICIES_PER_SPRITE,
               batch->indicies, GL_STATIC_DRAW);

  // Position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // Texture coordinates
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coords));
  glEnableVertexAttribArray(1);

  // Color
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void add_sprite(SpriteBatch *batch, Sprite *sprite) {
  if (batch->sprite_count >= MAX_SPRITES) {
    printf("Sprite batch full\n");
    return;
  }

  int start_vertex = batch->sprite_count * VERTICIES_PER_SPRITE;

  float half_width = sprite->size[0] / 2.0f;
  float half_height = sprite->size[1] / 2.0f;
  float positions[4][2] = {
      {-half_width, -half_height},
      {half_width, -half_height},
      {half_width, half_height},
      {-half_width, half_height},
  };

  if (sprite->rotation != 0.0f) {
    float radians = sprite->rotation * (GLM_PI / 180.0f);
    float cos_theta = cosf(radians);
    float sin_theta = sinf(radians);

    for (int i = 0; i < VERTICIES_PER_SPRITE; i++) {
      float x = positions[i][0];
      float y = positions[i][1];
      positions[i][0] = x * cos_theta - y * sin_theta;
      positions[i][1] = x * sin_theta + y * cos_theta;
    }
  }

  for (int i = 0; i < VERTICIES_PER_SPRITE; i++) {
    Vertex vertex;
    vertex.position[0] = positions[i][0] + sprite->position[0];
    vertex.position[1] = positions[i][1] + sprite->position[1];
    vertex.tex_coords[0] =
        (i == 0 || i == 3) ? sprite->tex_coords[0] : sprite->tex_coords[2];
    vertex.tex_coords[1] =
        (i == 0 || i == 1) ? sprite->tex_coords[1] : sprite->tex_coords[3];
    // Set color
    memcpy(vertex.color, sprite->color, sizeof(sprite->color));
    batch->verticies[start_vertex + i] = vertex;
  }

  batch->sprite_count++;
}

void update_sprite(SpriteBatch *batch, int index, Sprite *sprite) {
  if (index < 0 || index >= batch->sprite_count) {
    printf("Invalid sprite index\n");
    return;
  }

  int start_vertex = index * VERTICIES_PER_SPRITE;

  float half_width = sprite->size[0] / 2.0f;
  float half_height = sprite->size[1] / 2.0f;
  float positions[4][2] = {
      {-half_width, -half_height},
      {half_width, -half_height},
      {half_width, half_height},
      {-half_width, half_height},
  };

  if (sprite->rotation != 0.0f) {
    float radians = sprite->rotation * (GLM_PI / 180.0f);
    float cos_theta = cosf(radians);
    float sin_theta = sinf(radians);

    for (int i = 0; i < VERTICIES_PER_SPRITE; i++) {
      float x = positions[i][0];
      float y = positions[i][1];
      positions[i][0] = x * cos_theta - y * sin_theta;
      positions[i][1] = x * sin_theta + y * cos_theta;
    }
  }

  for (int i = 0; i < VERTICIES_PER_SPRITE; i++) {
    Vertex vertex;
    vertex.position[0] = positions[i][0] + sprite->position[0];
    vertex.position[1] = positions[i][1] + sprite->position[1];
    vertex.tex_coords[0] =
        (i == 0 || i == 3) ? sprite->tex_coords[0] : sprite->tex_coords[2];
    vertex.tex_coords[1] =
        (i == 0 || i == 1) ? sprite->tex_coords[1] : sprite->tex_coords[3];
    // Set color
    memcpy(vertex.color, sprite->color, sizeof(sprite->color));
    batch->verticies[start_vertex + i] = vertex;
  }
}

void draw_sprite_batch(SpriteBatch *batch) {
  mat4 projection;
  mat4 transform;
  glm_ortho(0.0f, 640.0f, 480.0f, 0.0f, -1.0f, 1.0f, projection);
  glm_mat4_identity(transform);
  glBindVertexArray(batch->vao);

  glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0,
                  sizeof(Vertex) * batch->sprite_count * VERTICIES_PER_SPRITE,
                  batch->verticies);

  glUseProgram(batch->shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, batch->texture);

  GLint projection_location = glGetUniformLocation(batch->shader, "projection");
  GLint transform_location = glGetUniformLocation(batch->shader, "transform");
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, (float *)projection);
  glUniformMatrix4fv(transform_location, 1, GL_FALSE, (float *)transform);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->ebo);
  glDrawElements(GL_TRIANGLES, batch->sprite_count * INDICIES_PER_SPRITE,
                 GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

void set_sprite_tex_coords(Sprite *sprite, float atlas_width,
                           float atlas_height, float x, float y, float width,
                           float height) {
  sprite->tex_coords[0] = x / atlas_width;
  sprite->tex_coords[1] = y / atlas_height;
  sprite->tex_coords[2] = (x + width) / atlas_width;
  sprite->tex_coords[3] = (y + height) / atlas_height;
}