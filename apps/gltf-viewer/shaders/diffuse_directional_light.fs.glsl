#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

uniform vec3 uLightDir;
uniform vec3 uLightInt;

out vec3 fColor;

void main()
{
    // Need another normalization because interpolation of vertex attributes does not maintain unit length
    vec3 viewSpaceNormal = normalize(vViewSpaceNormal);
    float piDiv = 1. / 3.14;
    fColor = vec3(piDiv) * uLightInt * dot(viewSpaceNormal, uLightDir);
}