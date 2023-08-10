#ifndef _H_RAYMATH
#define _H_RAYMATH

#include<stdint.h>
#include<float.h>
#include <xmmintrin.h>

#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX

typedef float f32;
typedef uint32_t u32;
typedef uint8_t u8;

typedef struct
{
    f32 x, y;
}v2;

extern inline v2 V2(f32 A, f32 B)
{
    v2 result;
    result.x = A;
    result.y = B;
    return(result);
}

extern inline v2 v2_add(v2 A, v2 B)
{
    v2 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    return(result);
}

extern inline v2 v2_sub(v2 A, v2 B)
{
    v2 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    return(result);
}

extern inline v2 v2_neg(v2 A)
{
    v2 result;
    result.x = -A.x;
    result.y = -A.y;
    return(result);
}

extern inline v2 v2_scalar_mul(v2 A, f32 S)
{
    v2 result;
    result.x = A.x * S;
    result.y = A.y * S;

    return(result);
}

typedef struct
{
    f32 x, y, z;
}v3;

extern inline v3 V3(f32 A, f32 B, f32 C)
{
    v3 result;
    result.x = A;
    result.y = B;
    result.z = C;
    return(result);
}

extern inline v3 v3_add(v3 A, v3 B)
{
    v3 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;
    return(result);
}

extern inline v3 v3_sub(v3 A, v3 B)
{
    v3 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;
    return(result);
}

extern inline v3 v3_neg(v3 A)
{
    v3 result;
    result.x = -A.x;
    result.y = -A.y;
    result.z = -A.z;
    return(result);
}

extern inline v3 v3_scalar_mul(v3 A, f32 S)
{
    v3 result;
    result.x = A.x * S;
    result.y = A.y * S;
    result.z = A.z * S;

    return(result);
}

extern inline v3 v3_scalar_div(v3 A, f32 S)
{
    v3 result;
    f32 inv = 1/S;
    result.x = A.x * inv;
    result.y = A.y * inv;
    result.z = A.z * inv;

    return(result);
}

extern inline v3 v3_scalar_add(v3 A, f32 S)
{
    v3 result;
    result.x = A.x + S;
    result.y = A.y + S;
    result.z = A.z + S;

    return(result);
}

extern inline f32 dot(v3 A, v3 B)
{
    f32 result = A.x * B.x + A.y * B.y + A.z * B.z;
    return(result);
}

extern inline v3 cross(v3 A, v3 B)
{
    v3 result;

    result.x = A.y * B.z - A.z * B.y;
    result.y = A.z * B.x - A.x * B.z;
    result.z = A.x * B.y - A.y * B.x;

    return(result);
}

extern inline f32 square(f32 F32)
{
    f32 result = F32 * F32;
    return(result);
}

extern inline f32 square_root(f32 F32)
{
    f32 result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(F32)));
    return(result);
}

extern inline f32 length_sq(v3 A)
{
    f32 result = dot(A, A);
    return(result);
}

extern inline f32 length(v3 A)
{
    f32 result = square_root(length_sq(A));
    return(result);
}

extern inline v3 normalize(v3 A)
{
    v3 result;
    v3_scalar_mul(A, (1/length(A)));
    return(result);
}

extern inline v3 NOZ(v3 A)
{
    v3 result = {};
    f32 len_sq = length_sq(A);
    if(len_sq > square(0.0001f))
    {
        result = v3_scalar_mul(A, (1.0f/square_root(len_sq)));
    }
    return(result);
}



typedef struct
{
    f32 x, y, z, a;
}v4;

extern inline v4 V4(f32 A, f32 B, f32 C, f32 D)
{
    v4 result;

    result.x = A;
    result.y = B;
    result.z = C;
    result.a = D;

    return(result);
}

#include<math.h>
extern inline f32 POW(f32 B, f32 P)
{
    f32 result = (f32)pow(B, P);

    return(result);
}

// NOTE: Don't really know if the disparity between big and little endian
// id Correct
extern inline u32 pack_color_le(v3 color)
{
    u32 result;

    u32 r = (u32)(color.x * 255);
    u32 g = (u32)(color.y * 255);
    u32 b = (u32)(color.z * 255);

    result = (b << 8*2) | 
             (g << 8*1) | 
             (r << 8*0);

    return(result);
}

extern inline u32 pack_color_be(v3 color)
{
    u32 result;

    u32 r = (u32)(color.x * 255);
    u32 g = (u32)(color.y * 255);
    u32 b = (u32)(color.z * 255);

    result = (b << 8*0) | 
             (g << 8*1) | 
             (r << 8*2);

    return(result);
}
extern inline f32 FRAND()
{
    f32 result = rand()/((f32)RAND_MAX+1.0f);
    return(result);
}

#endif
