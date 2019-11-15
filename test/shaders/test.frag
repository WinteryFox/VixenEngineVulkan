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
const float g2 = g * g;
const vec3 sunPosition = vec3(0.0, 0.0, -1.0);
const vec3 _betaR = vec3(1.95e-2, 1.1e-1, 2.94e-1);
const vec3 _betaM = vec3(4e-2, 4e-2, 4e-2);

vec3 ACESFilm(vec3 x) {
    float tA = 2.51;
    float tB = 0.03;
    float tC = 2.43;
    float tD = 0.59;
    float tE = 0.14;
    return clamp((x*(tA*x+tB))/(x*(tC*x+tD)+tE), 0.0, 1.0);
}

void main() {
    vec4 clipSpace = projection * view * vec4(sunPosition, 1.0);
    vec2 screenSpace = clipSpace.xy / clipSpace.z * 0.5 + 0.5;
    vec2 uv = vec2(gl_FragCoord.x / 1280, gl_FragCoord.y / 720);

    float sR = rayleighAtt / uv.y;
    float sM = mieAtt / uv.y;

    float cosine = 1.0 - distance(screenSpace, uv);
    float fcos2 = cosine * cosine;
    vec3 extinction = exp(-(_betaR * sR + _betaM * sM));
    float miePhase = mie * pow(1.0 + g2 + 2.0 * g * cosine, -1.5) * (1.0 - g2) / (2.0 + g2);

    vec3 inScatter = (1.0 + fcos2) * rayleigh + _betaM / _betaR * miePhase;

    // Sky color
    vec3 color = inScatter * (1.0 - extinction);
    // Sun
    color += 0.47 * vec3(1.6, 1.4, 1.0) * pow(cosine, 350.0) * extinction;
    // Sun haze
    //color += 0.4 * vec3(0.8, 0.9, 1.0) * pow(cosine, 2.0) * extinction;

    // Adjust gamma
    color = pow(ACESFilm(color), vec3(gamma));

    outColor = vec4(color, 1.0);
}