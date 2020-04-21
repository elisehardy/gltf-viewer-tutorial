#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;
in vec4 vPosLightSpace;

uniform float uDirLightIntensity;
uniform float uAmbLightIntensity;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;

uniform bool uShadowMapEnabled;
uniform mat4 uDirLightViewProjMatrix;
uniform sampler2D uShadowMap;
uniform float uShadowMapBias;

uniform vec3 uEmissiveFactor;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;
uniform vec4 uBaseColorFactor;
uniform sampler2D uBaseColorTexture;
uniform sampler2D uMetallicRoughnessTexture;
uniform sampler2D uEmissiveTexture;

uniform sampler2D uOcclusionTexture;
uniform float uOcclusionStrength;

out vec3 fColor;

// Constants
const float GAMMA = 2.2;
const float INV_GAMMA = 1. / GAMMA;
const float M_PI = 3.141592653589793;
const float M_1_PI = 1.0 / M_PI;
const vec3 dielectricSpecular = vec3(0.04, 0.04, 0.04);
const vec3 black = vec3(0, 0, 0);

// We need some simple tone mapping functions
// Basic gamma = 2.2 implementation
// Stolen here: https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/src/shaders/tonemapping.glsl

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 LINEARtoSRGB(vec3 color) {
    return pow(color, vec3(INV_GAMMA));
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec4 SRGBtoLINEAR(vec4 srgbIn) {
    return vec4(pow(srgbIn.xyz, vec3(GAMMA)), srgbIn.w);
}

float computeShadow() {
    float shadow = 0.f;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for(int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec4 positionInDirLightScreen = uDirLightViewProjMatrix * vec4(vViewSpacePosition, 1);
            vec3 positionInDirLightNDC = vec3(positionInDirLightScreen / positionInDirLightScreen.w) * 0.5 + 0.5;
            float depthBlockerInDirSpace = texture(uShadowMap, positionInDirLightNDC.xy + vec2(x, y) * texelSize).r;

            shadow += positionInDirLightNDC.z < depthBlockerInDirSpace + uShadowMapBias ? 1.0 : 0.0;
        }
    }

    return shadow / 25.f;
}

void main() {
    vec3 N = normalize(vViewSpaceNormal);
    vec3 L = uLightDirection;
    vec3 V = normalize(-vViewSpacePosition);
    vec3 H = normalize(L + V);

    vec4 baseColorFromTexture = SRGBtoLINEAR(texture(uBaseColorTexture, vTexCoords));
    vec4 emissiveTexture = SRGBtoLINEAR(texture(uEmissiveTexture, vTexCoords));
    vec4 baseColor = baseColorFromTexture * uBaseColorFactor;
    vec4 metallicRougnessFromTexture = texture(uMetallicRoughnessTexture, vTexCoords);
    float roughness = uRoughnessFactor * metallicRougnessFromTexture.g;
    vec3 metallic = vec3(uMetallicFactor * metallicRougnessFromTexture.b);
    vec3 emissive = uEmissiveFactor * emissiveTexture.rgb;

    vec4 occlusionFromTexture = SRGBtoLINEAR(texture(uOcclusionTexture, vTexCoords));

    vec3 c_diff = mix(baseColor.rgb * (1 - dielectricSpecular.r), black, metallic);
    vec3 F_0 = mix(dielectricSpecular, baseColor.rgb, metallic);

    float alpha = roughness * roughness;

    float NdotL = clamp(dot(N, L), 0.0, 1.0);
    float NdotV = clamp(dot(N, V), 0.0, 1.0);
    float NdotH = clamp(dot(N, H), 0.0, 1.0);


    vec3 diffuse = c_diff * M_1_PI;

    float Vis = 0;
    float alpha_alpha = alpha * alpha;
    float denumVis = (
        (NdotL) *sqrt((NdotV) * (NdotV) * (1 - alpha_alpha) + alpha_alpha)
        + (NdotV) * sqrt((NdotL * NdotL) *  (1 - alpha_alpha) + alpha_alpha)
    );
    if (denumVis > 0){
        Vis =  0.5 / (denumVis);
    }

    float VdotH = clamp(dot(V, H), 0.0, 1.0);
    float baseShlickFactor = (1 - VdotH);
    float shlickFactor  = baseShlickFactor * baseShlickFactor;
    shlickFactor *= shlickFactor;
    shlickFactor *= baseShlickFactor;
    vec3 F = F_0 + (vec3(1) - F_0) * shlickFactor;
    vec3 f_diffuse = (1 - F) * diffuse;

    float denumD_witout_PI = ((NdotH*NdotH) * (alpha_alpha -1) + 1);
    float denumD = M_PI * denumD_witout_PI*denumD_witout_PI;
    float D = alpha_alpha / denumD;
    vec3 f_specular =  F * Vis * D;
    vec3 f = f_diffuse + f_specular;

    float shadow = uShadowMapEnabled ? computeShadow() : 0.f;

    fColor = (
        (f_diffuse + f_specular)
        * max(uDirLightIntensity * NdotL * (1.0 - shadow) , uAmbLightIntensity)
        * uLightColor
        + emissive
    );
//    fColor = mix(fColor, fColor * occlusionFromTexture.r, uOcclusionStrength);
    fColor = LINEARtoSRGB(fColor);
}
