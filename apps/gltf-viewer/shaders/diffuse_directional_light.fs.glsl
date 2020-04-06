#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

uniform float uDirLightIntensity;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;

out vec3 fColor;

void main() {
    fColor= vec3(1. / 3.14) * uDirLightIntensity * uLightColor * dot(normalize(vViewSpaceNormal), uLightDirection);
}
