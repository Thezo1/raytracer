#ifndef _RAY_H_
#define _RAY_H_

#include "ray_math.h"
typedef struct
{
    v3 color;
    f32 specular;
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
    u32 light_index;
    f32 i;
    v3 direction;
    v3 position;
}Light;

typedef struct
{
    u32 material_count;
    Material *materials;

    u32 plane_count;
    Plane *planes;

    u32 sphere_count;
    Sphere *spheres;

    u32 light_count;
    Light *lights;
}World;
#endif
