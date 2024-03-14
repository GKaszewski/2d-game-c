#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"

int main(void) {
  GLFWwindow *window;
  if (!glfwInit()) {
    return -1;
  }

  window = glfwCreateWindow(640, 480, "My dream game", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    return -1;
  }

  glfwSwapInterval(1);

  GLuint shader_id =
      load_shaders("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");
  SpriteBatch batch;
  init_sprite_batch(&batch, "assets/textures/tileset.png");
  batch.shader = shader_id;

  Sprite grass = {
      {320.0f, 240.0f}, {16.0f, 16.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, 0.0f};
  set_sprite_tex_coords(&grass, 160, 112, 0, 0, 16, 16);
  add_sprite(&batch, &grass);

  Sprite better_grass = {
      {336.0f, 240.0f}, {16.0f, 16.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {}, 45.0f};
  set_sprite_tex_coords(&better_grass, 160, 112, 0, 96, 16, 16);
  add_sprite(&batch, &better_grass);

  int x = 0;
  int y = 0;

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grass.position[0] = x + 320.0f;

    // set_sprite_tex_coords(&grass, 160, 112, x, y, 16, 16);
    update_sprite(&batch, 0, &grass);
    draw_sprite_batch(&batch);

    glfwSwapBuffers(window);
    glfwPollEvents();

    x += 1;
    if (x >= 160) {
      x = 0;
      y += 16;
    }
    if (y >= 112) {
      y = 0;
    }
  }

  glfwTerminate();
  return 0;
}