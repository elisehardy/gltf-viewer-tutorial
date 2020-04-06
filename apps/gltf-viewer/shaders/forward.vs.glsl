#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec2 vTexCoords;
out vec4 vPosLightSpace;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uDirLightViewProjMatrix;

void main()
{
    vViewSpacePosition = vec3(uMVMatrix * vec4(aPosition, 1));
    vViewSpaceNormal = normalize(vec3(uNormalMatrix * vec4(aNormal, 0)));
    vTexCoords = aTexCoords;
    vPosLightSpace = uDirLightViewProjMatrix * vec4(vViewSpacePosition, 1.0);
    gl_Position =  uMVPMatrix * vec4(aPosition, 1);
}
