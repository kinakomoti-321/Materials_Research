#pragma once
#include <cmath>
#include <glm/glm.hpp>

using namespace glm;
namespace MR
{
    // https://www.shadertoy.com/view/WdjSW3
    vec3 toneMappting_Uchimura(const vec3 &col)
    {
        const float P = 1.0;
        const float a = 1.0;
        const float m = 0.22;
        const float l = 0.4;
        const float c = 1.33;
        const float b = 0.0;

        float l0 = ((P - m) * l) / a;
        float L0 = m - m / a;
        float L1 = m + (1.0 - m) / a;
        float S0 = m + l0;
        float S1 = m + a * l0;
        float C2 = (a * P) / (P - S1);
        float CP = -C2 / P;

        vec3 w0 = vec3(1.0) - smoothstep(vec3(0.0), vec3(m), col);
        vec3 w2 = step(m + l0, col);
        vec3 w1 = vec3(1.0) - w0 - w2;

        vec3 T = m * pow(col / m, vec3(c)) + b;
        vec3 S = P - (P - S1) * exp(CP * (col - S0));
        vec3 L = m + a * (col - vec3(m));

        return T * w0 + L * w1 + S * w2;
    }
}