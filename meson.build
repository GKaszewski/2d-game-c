project('2d_side_scroller', 'c', version : '0.1', default_options : ['c_std=c11'])

# dependencies
glfw_dep = dependency('glfw3', version : '>= 3.3')
glew_dep = dependency('glew', version : '>= 2.0')
cglm_dep = dependency('cglm')
external_dir = include_directories('external')

executable('game',
  sources : ['./src/main.c', './src/draw.c', './src/draw.h'],
  include_directories : [external_dir],
  dependencies : [glfw_dep, glew_dep, cglm_dep],
  link_args: ['-lm'],
  install : true
)