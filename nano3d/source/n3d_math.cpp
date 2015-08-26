#include "n3d_math.h"

void n3d_identity(mat4f_t & m) {

    float * e = m.e;
    e[0x0] = 1.f; e[0x1] = 0.f; e[0x2] = 0.f; e[0x3] = 0.f;
    e[0x4] = 0.f; e[0x5] = 1.f; e[0x6] = 0.f; e[0x7] = 0.f;
    e[0x8] = 0.f; e[0x9] = 0.f; e[0xa] = 1.f; e[0xb] = 0.f;
    e[0xc] = 0.f; e[0xd] = 0.f; e[0xe] = 0.f; e[0xf] = 1.f;
}

void n3d_frustum(
    mat4f_t & m,
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far) {

    float xx = float((2.f * near) / (right - left));
    float yy = float((2.f * near) / (top - bottom));
    float  A = float((right + left) / (right - left));
    float  B = float((top + bottom) / (top - bottom));
    float  C = float(-(far + near) / (far - near));
    float  D = float(-(2.f*far*near) / (far - near));

    float * e = m.e;

    e[0x0] = xx;  e[0x4] = 0.f; e[0x8] = A;   e[0xc] = 0.f;
    e[0x1] = 0.f; e[0x5] = yy;  e[0x9] = B;   e[0xd] = 0.f;
    e[0x2] = 0.f; e[0x6] = 0.f; e[0xa] = C;   e[0xe] = D;
    e[0x3] = 0.f; e[0x7] = 0.f; e[0xb] = -1.f; e[0xf] = 0.f;
}
