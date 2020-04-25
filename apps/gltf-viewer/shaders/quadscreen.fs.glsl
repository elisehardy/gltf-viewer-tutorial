#version 330

in vec2 vTexCoords;

uniform sampler2D uTex;

out vec4 fColor;

void main() {
    fColor = vec4(1);
//    fColor = vec4(vec3(texture(uTex, vTexCoords).r), 1.0);
}
