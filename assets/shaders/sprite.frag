#version 330 core

in vec2 TexCoord; // Texture coordinates from vertex shader
in vec4 VertexColor; // Vertex color from vertex shader

out vec4 FragColor; // Final fragment color

uniform sampler2D spriteTexture; // The texture atlas
uniform float globalOpacity; // Global opacity for fading sprites

void main() {
    vec4 texColor = texture(spriteTexture, TexCoord);
    FragColor = texColor * VertexColor;
    FragColor.a *= globalOpacity; // Apply global opacity
}
