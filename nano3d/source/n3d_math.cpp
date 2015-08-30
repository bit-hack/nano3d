#include <math.h>
#include "n3d_math.h"

// little macro to make matrix index easier
// also allowing easy way to transpose
#define IX(x,y) (x*4+y)

void n3d_identity(mat4f_t & m) {

    float * e = m.e;
    e[0x0] = 1.f; e[0x1] = 0.f; e[0x2] = 0.f; e[0x3] = 0.f;
    e[0x4] = 0.f; e[0x5] = 1.f; e[0x6] = 0.f; e[0x7] = 0.f;
    e[0x8] = 0.f; e[0x9] = 0.f; e[0xa] = 1.f; e[0xb] = 0.f;
    e[0xc] = 0.f; e[0xd] = 0.f; e[0xe] = 0.f; e[0xf] = 1.f;
}

// OpenGL perspective projection matrix
void n3d_frustum(
    mat4f_t & m,
    const float l,
    const float r,
    const float b,
    const float t,
    const float n,
    const float f)
{
    float * M = m.e;
    
    M[IX(0, 0)] = 2 * n / (r - l);
    M[IX(0, 1)] = 0;
    M[IX(0, 2)] = 0;
    M[IX(0, 3)] = 0;

    M[IX(1, 0)] = 0;
    M[IX(1, 1)] = 2 * n / (t - b);
    M[IX(1, 2)] = 0;
    M[IX(1, 3)] = 0;

    M[IX(2, 0)] = (r + l) / (r - l);
    M[IX(2, 1)] = (t + b) / (t - b);
    M[IX(2, 2)] =-(f + n) / (f - n);
    M[IX(2, 3)] =-1;

    M[IX(3, 0)] = 0;
    M[IX(3, 1)] = 0;
    M[IX(3, 2)] =-(2 * f * n) / (f - n);
    M[IX(3, 3)] = 0;
}

/* generate a rotation matrix */
void n3d_rotate(
    mat4f_t & m,
    const float a, 
    const float b, 
    const float c) 
{
    const float sa = sinf(a), ca = cosf(a);
    const float sb = sinf(b), cb = cosf(b);
    const float sc = sinf(c), cc = cosf(c);

    float * M = m.e;

    M[IX(0, 0)] = cc*cb;
    M[IX(0, 1)] = ca*sc*cb + sa*sb;
    M[IX(0, 2)] = sa*sc*cb - ca*sb;
    M[IX(0, 3)] = 0.f;
    
    M[IX(1, 0)] = -sc;
    M[IX(1, 1)] = ca*cc;
    M[IX(1, 2)] = sa*cc;
    M[IX(1, 3)] = 0.f;
    
    M[IX(2, 0)] = cc*sb;
    M[IX(2, 1)] = ca*sc*sb - sa*cb;
    M[IX(2, 2)] = sa*sc*sb + ca*cb;
    M[IX(2, 3)] = 0.f;
    
    M[IX(3, 0)] = 0.f;
    M[IX(3, 1)] = 0.f;
    M[IX(3, 2)] = 0.f;
    M[IX(3, 3)] = 1.f;
}

void n3d_translate(
    mat4f_t & m,
    const vec3f_t & p) {

    float * M = m.e;

    M[IX(3, 0)] = p.x;
    M[IX(3, 1)] = p.y;
    M[IX(3, 2)] = p.z;
    M[IX(3, 3)] = 1.f;
}
