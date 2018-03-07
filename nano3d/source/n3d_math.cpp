// n3d_math.cpp
//   implement nano3d math helper routines

#include <cmath>
#include <array>

#include "n3d_math.h"

void n3d_identity(mat4f_t& m)
{
    m(0x0) = 1.f; m(0x1) = 0.f; m(0x2) = 0.f; m(0x3) = 0.f;
    m(0x4) = 0.f; m(0x5) = 1.f; m(0x6) = 0.f; m(0x7) = 0.f;
    m(0x8) = 0.f; m(0x9) = 0.f; m(0xa) = 1.f; m(0xb) = 0.f;
    m(0xc) = 0.f; m(0xd) = 0.f; m(0xe) = 0.f; m(0xf) = 1.f;
}

void n3d_frustum(
    mat4f_t& m,
    const float l,
    const float r,
    const float b,
    const float t,
    const float n,
    const float f)
{
    m(0, 0) = 2 * n / (r - l);
    m(0, 1) = 0;
    m(0, 2) = 0;
    m(0, 3) = 0;

    m(1, 0) = 0;
    m(1, 1) = 2 * n / (t - b);
    m(1, 2) = 0;
    m(1, 3) = 0;

    m(2, 0) = (r + l) / (r - l);
    m(2, 1) = (t + b) / (t - b);
    m(2, 2) = -(f + n) / (f - n);
    m(2, 3) = -1;

    m(3, 0) = 0;
    m(3, 1) = 0;
    m(3, 2) = -(2 * f * n) / (f - n);
    m(3, 3) = 0;
}

void n3d_rotate(
    mat4f_t& m,
    const float a,
    const float b,
    const float c)
{
    const float sa = sinf(a), ca = cosf(a);
    const float sb = sinf(b), cb = cosf(b);
    const float sc = sinf(c), cc = cosf(c);

    m(0, 0) = cc * cb;
    m(0, 1) = ca * sc * cb + sa * sb;
    m(0, 2) = sa * sc * cb - ca * sb;
    m(0, 3) = 0.f;

    m(1, 0) = -sc;
    m(1, 1) = ca * cc;
    m(1, 2) = sa * cc;
    m(1, 3) = 0.f;

    m(2, 0) = cc * sb;
    m(2, 1) = ca * sc * sb - sa * cb;
    m(2, 2) = sa * sc * sb + ca * cb;
    m(2, 3) = 0.f;

    m(3, 0) = 0.f;
    m(3, 1) = 0.f;
    m(3, 2) = 0.f;
    m(3, 3) = 1.f;
}

void n3d_translate(
    mat4f_t& m,
    const vec3f_t& p)
{
    m(3, 0) = p.x;
    m(3, 1) = p.y;
    m(3, 2) = p.z;
    m(3, 3) = 1.f;
}

void n3d_transform(
    const uint32_t num_verts,
    const mat4f_t& m,
    const vec4f_t* in,
    vec4f_t* out)
{
    for (uint32_t q = 0; q < num_verts; ++q) {
        //(todo): unroll and use SIMD instructions
        const vec4f_t& s = in[q];
        out[q] = vec4<float>(
            s.x * m(0, 0) + s.y * m(1, 0) + s.z * m(2, 0) + s.w * m(3, 0),
            s.x * m(0, 1) + s.y * m(1, 1) + s.z * m(2, 1) + s.w * m(3, 1),
            s.x * m(0, 2) + s.y * m(1, 2) + s.z * m(2, 2) + s.w * m(3, 2),
            s.x * m(0, 3) + s.y * m(1, 3) + s.z * m(2, 3) + s.w * m(3, 3));
    }
}

bool n3d_invert(mat4f_t& m)
{
    std::array<float, 16> i;

#define I(a, b, c) (m(a) * m(b) * m(c))
    i[0]  =  I(5, 10,  15) - I(5,  11, 14) - I(9,  6, 15) + 
             I(9,  7,  14) + I(13, 6,  11) - I(13, 7, 10);
    i[4]  = -I(4, 10,  15) + I(4,  11, 14) + I(8,  6, 15) -
             I(8,  7,  14) - I(12, 6,  11) + I(12, 7, 10);
    i[8]  =  I(4,  9,  15) - I(4,  11, 13) - I(8,  5, 15) +
             I(8,  7,  13) + I(12, 5,  11) - I(12, 7, 9 );
    i[12] = -I(4,  9,  14) + I(4,  10, 13) + I(8,  5, 14) -
             I(8,  6,  13) - I(12, 5,  10) + I(12, 6, 9 );
    i[1]  = -I(1,  10, 15) + I(1,  11, 14) + I(9,  2, 15) -
             I(9,  3,  14) - I(13, 2,  11) + I(13, 3, 10);
    i[5]  =  I(0,  10, 15) - I(0,  11, 14) - I(8,  2, 15) +
             I(8,  3,  14) + I(12, 2,  11) - I(12, 3, 10);
    i[9]  = -I(0,  9,  15) + I(0,  11, 13) + I(8,  1, 15) -
             I(8,  3,  13) - I(12, 1,  11) + I(12, 3, 9 );
    i[13] =  I(0,  9,  14) - I(0,  10, 13) - I(8,  1, 14) +
             I(8,  2,  13) + I(12, 1,  10) - I(12, 2, 9 );
    i[2]  =  I(1,  6,  15) - I(1,  7,  14) - I(5,  2, 15) +
             I(5,  3,  14) + I(13, 2,  7 ) - I(13, 3, 6 );
    i[6]  = -I(0,  6,  15) + I(0,  7,  14) + I(4,  2, 15) -
             I(4,  3,  14) - I(12, 2,  7 ) + I(12, 3, 6 );
    i[10] =  I(0,  5,  15) - I(0,  7,  13) - I(4,  1, 15) +
             I(4,  3,  13) + I(12, 1,  7 ) - I(12, 3, 5 );
    i[14] = -I(0,  5,  14) + I(0,  6,  13) + I(4,  1, 14) -
             I(4,  2,  13) - I(12, 1,  6 ) + I(12, 2, 5 );
    i[3]  = -I(1,  6,  11) + I(1,  7,  10) + I(5,  2, 11) -
             I(5,  3,  10) - I(9 , 2,  7 ) + I(9,  3, 6 );
    i[7]  =  I(0,  6,  11) - I(0,  7,  10) - I(4,  2, 11) +
             I(4,  3,  10) + I(8 , 2,  7 ) - I(8,  3, 6 );
    i[11] = -I(0,  5,  11) + I(0,  7,  9 ) + I(4,  1, 11) -
             I(4,  3,  9 ) - I(8 , 1,  7 ) + I(8,  3, 5 );
    i[15] =  I(0,  5,  10) - I(0,  6,  9 ) - I(4,  1, 10) + 
             I(4,  2,  9 ) + I(8 , 1,  6 ) - I(8,  2, 5 );
#undef I

    const float det = m(0) * i[0] +
                      m(1) * i[4] +
                      m(2) * i[8] +
                      m(3) * i[12];
    if (det == 0)
        return false;
    const float rdet = 1.0f / det;
    for (int j = 0; j < 16; j++)
        m(j)  = i[j] * rdet;
    return true;
}

void n3d_transpose(mat4f_t& m)
{
    std::swap(m(0x1), m(0x4));
    std::swap(m(0x2), m(0x8));
    std::swap(m(0x3), m(0xc));
    std::swap(m(0x6), m(0x9));
    std::swap(m(0x7), m(0xd));
    std::swap(m(0xb), m(0xe));
}

void n3d_multiply(
    mat4f_t& a,
    const mat4f_t& b)
{
    mat4f_t r;
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            float& v = r(i, j);
            v  = a(i, 0) * b(0, j);
            v += a(i, 1) * b(1, j);
            v += a(i, 2) * b(2, j);
            v += a(i, 3) * b(3, j);
        }
    }
    a = r;
}

void n3d_rotatef(
    mat4f_t& mp,
    const float angle,
    float x,
    float y,
    float z)
{
    const float l = 1.f / sqrtf(x * x + y * y + z * z);
    (x *= l), (y *= l), (z *= l);

    const float r = angle * (n3d_pi2 / 360.f);
    const float c = cosf(r);
    const float s = sinf(r);
    const float t = 1 - c;

    mat4f_t m;
    m(0x0) = x*x*t+c;   m(0x4) = x*y*t-z*s; m(0x8) = x*z*t+y*s; m(0xc) = 0.f;
    m(0x1) = y*x*t+z*s; m(0x5) = y*y*t+c;   m(0x9) = y*z*t-x*s; m(0xd) = 0.f;
    m(0x2) = x*z*t-y*s; m(0x6) = y*z*t+x*s; m(0xa) = z*z*t+c;   m(0xe) = 0.f;
    m(0x3) = 0.f;       m(0x7) = 0.f;       m(0xb) = 0.f;       m(0xf) = 1.f;
    n3d_multiply(mp, m);
}

void n3d_scalef(mat4f_t &mp, float x, float y, float z)
{
    mat4f_t m;
    m(0x0) = x;   m(0x4) = 0.f; m(0x8) = 0.f; m(0xc) = 0.f;
    m(0x1) = 0.f; m(0x5) = y;   m(0x9) = 0.f; m(0xd) = 0.f;
    m(0x2) = 0.f; m(0x6) = 0.f; m(0xa) = z;   m(0xe) = 0.f;
    m(0x3) = 0.f; m(0x7) = 0.f; m(0xb) = 0.f; m(0xf) = 1.f;
    n3d_multiply(mp, m);
}

void n3d_translatef(mat4f_t &mp, float x, float y, float z)
{
    mat4f_t m;
    m(0x0) = 1.f; m(0x4) = 0.f; m(0x8) = 0.f; m(0xc) = x;
    m(0x1) = 0.f; m(0x5) = 1.f; m(0x9) = 0.f; m(0xd) = y;
    m(0x2) = 0.f; m(0x6) = 0.f; m(0xa) = 1.f; m(0xe) = z;
    m(0x3) = 0.f; m(0x7) = 0.f; m(0xb) = 0.f; m(0xf) = 1.f;
    n3d_multiply(mp, m);
}
