#ifndef _RAY_H_
#define _RAY_H_

#include "ray_math.h"
typedef struct
{
    v3 color;
}Material;

typedef struct
{
    v3 N;
    f32 d;
    u32 mat_index;
}Plane;

typedef struct
{
    v3 point;
    f32 r;
    u32 mat_index;
}Sphere;

typedef struct
{
    u32 material_count;
    Material *materials;

    u32 plane_count;
    Plane *planes;

    u32 sphere_count;
    Sphere *spheres;
}World;
#endif
