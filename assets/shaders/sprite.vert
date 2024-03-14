#version 330 core

layout (location = 0) in vec2 inPosition; // Vertex position (x, y)
layout (location = 1) in vec2 inTexCoord; // Texture coordinates (u, v)
layout (location = 2) in vec4 inColor; // Vertex color

out vec2 TexCoord; // Pass texture coordinates to fragment shader
out vec4 VertexColor; // Pass vertex color to fragment shader

uniform mat4 projection;
uniform mat4 transform; // Transformation matrix

void main() {
    gl_Position = projection * transform * vec4(inPosition, 0.0, 1.0);
    TexCoord = inTexCoord;
    VertexColor = inColor;
}