#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in mat4 projection;
layout(location = 5) in mat4 view;

const float gamma = 1.1;
const float rayleigh = 1.1;
const float mie = 1.0;
const float rayleighAtt = 1.0;
const float mieAtt = 1.2;
const float g = -0.9;

vec3 _betaR = vec3(1.95e-2, 1.1e-1, 2.94e-1);
vec3 _betaM = vec3(4e-2, 4e-2, 4e-2);

const vec2 resolution = vec2(16.0, 9.0);

vec3 ACESFilm(vec3 x)
{
    float tA = 2.51;
    float tB = 0.03;
    float tC = 2.43;
    float tD = 0.59;
    float tE = 0.14;
    return clamp((x*(tA*x+tB))/(x*(tC*x+tD)+tE), 0.0, 1.0);
}

const vec3 sunPosition = vec3(-20.0, 7.5, -20.0);

void main() {
    float sR = rayleighAtt / 10.0;// TODO
    float sM = mieAtt / 10.0;// TODO

    vec4 extinction = vec4(exp(-(_betaR * sR + _betaM * sM)), 1.0);

    vec3 clipSpace = (projection * view * vec4(sunPosition, 1.0)).xyw;
    vec2 screenSpace = clipSpace.xy / clipSpace.z;
    vec2 fragmentPos = vec2(gl_FragCoord.x / 1280, gl_FragCoord.y / 720);

    if (length(screenSpace - fragmentPos) < 0.1)
        outColor = vec4(1.0, 1.0, 0.0, 1.0);
    else
        discard;
}