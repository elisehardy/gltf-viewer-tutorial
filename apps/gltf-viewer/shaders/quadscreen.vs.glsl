#version 330

layout (location = 0) in vec3 aVertexCoords;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;

void main() {
    vTexCoords = aTexCoords;
    gl_Position =  vec4(aVertexCoords, 1.0);
}
