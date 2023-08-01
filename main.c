#include<stdio.h>
#include<stdlib.h>
#include "ray_math.h"
#include "ray.h"

#define WIDTH 1280
#define HEIGHT 720

static u32 IMAGE[HEIGHT][WIDTH];

void fill_image()
{
    for(int y = 0; 
            y < HEIGHT; 
            ++y)
    {
        for(int x = 0; 
                x < WIDTH; 
                ++x)
        {
            if(y == (HEIGHT-1))
            {
                IMAGE[y][x] = 0xff00ff00;
            }
            else
            {
                IMAGE[y][x] = 0xff180018;
            }
        }
        
    }
}

void save_to_ppm(const char *filename)
{
    FILE *file;
    file = fopen("output.ppm", "wb");
    if(!file)
    {
        printf("Cannot open file");
        exit(1);
    }

    fprintf(file, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    for(int y = 0; 
            y < HEIGHT; 
            ++y)
    {
        for(int x = 0; 
                x < WIDTH; 
                ++x)
        {

            // TODO: try alpha blending
            u32 pixel = IMAGE[y][x];
            u8 bytes[3] = 
            {
                (pixel & 0x0000ff) >> 8*0,
                (pixel & 0x00ff00) >> 8*1,
                (pixel & 0xff0000) >> 8*2,
            };
            fwrite(bytes, sizeof(bytes), 1, file);

        }

    }
    fclose(file);
}

v3 ray_cast(World *world, v3 ray_origin, v3 ray_direction)
{
    v3 result = world->materials[0].color;
    f32 hit_distance = F32MAX;
    f32 tolerance = 0.0001f;

    for(u32 plane_index = 0;
            plane_index < world->plane_count;
            ++plane_index)
    {
        Plane plane = world->planes[plane_index];
        f32 denom = dot(plane.N, ray_direction);
        if((denom < -tolerance) > (denom > tolerance))
        {
            f32 t = -(plane.d + dot(plane.N, ray_origin)) / denom;
            if((t > 0) && (t < hit_distance))
            {
                hit_distance = t;
                result = world->materials[plane.mat_index].color;
            }
        }
    }

    for(u32 sphere_index = 0;
            sphere_index < world->sphere_count;
            ++sphere_index)
    {
        Sphere sphere = world->spheres[sphere_index];
        v3 co = v3_sub(ray_origin, sphere.point);

        f32 a = dot(ray_direction, ray_direction);
        f32 b = 2 * dot(co, ray_direction);
        f32 c = dot(co, co) - sphere.r * sphere.r;
        f32 discriminant = (b * b) - (4 * a * c);

        if(discriminant > tolerance)
        {
            f32 t1 = (-b + square_root(discriminant)) / (2 * a);
            f32 t2 = (-b - square_root(discriminant)) / (2 * a);

            f32 t = t2;
            if((t1 > 0) && (t1 < t2))
            {
                t = t1;
            }

            if((t > 0) && (t < hit_distance))
            {
                hit_distance = t;
                result = world->materials[sphere.mat_index].color;
            }
        }
    }

    return(result);
}

u32 pack_color(v3 color)
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

int main()
{
    Material materials[3] = {};
    materials[0].color = V3(0.1, 0.1, 0.1);
    materials[1].color = V3(1, 0, 0);
    materials[2].color = V3(0, 1, 0);

    Plane plane = {};
    plane.N = V3(0, 0, 1);
    plane.d = 0;
    plane.mat_index = 1;

    Sphere sphere = {};
    sphere.r = 1.0f;
    sphere.point = V3(0, 0, 0);
    sphere.mat_index = 2;

    World world = {};
    world.material_count = 1;
    world.materials = materials;
    world.plane_count = 1;
    world.planes = &plane;
    world.sphere_count = 1;
    world.spheres = &sphere;

    v3 camera_p = V3(0, -10, 1);
    v3 camera_z = NOZ(v3_sub(camera_p, V3(0, 0, 0)));
    v3 camera_x = NOZ(cross(camera_z, V3(0, 0, 1)));
    v3 camera_y = NOZ(cross(camera_z, camera_x));

    f32 film_dist = 1.0f;
    f32 film_width = 1.0f;
    f32 film_height = 1.0f;
    if(WIDTH > HEIGHT)
    {
        film_height = film_width * ((f32)HEIGHT / (f32)WIDTH);
    }
    else if(HEIGHT > WIDTH)
    {
        film_width = film_height * ((f32)WIDTH / (f32)HEIGHT);
    }

    f32 half_film_width = 0.5f*film_width;
    f32 half_film_heigth = 0.5f*film_width;
    v3 film_c = v3_sub(camera_p, v3_scalar_mul(camera_z, film_dist));

    for(int y = 0; 
            y < HEIGHT; 
            ++y)
    {
        f32 film_y = -1.0f + 2.0f*((f32)y / (f32) HEIGHT);
        for(int x = 0; 
                x < WIDTH; 
                ++x)
        {
            f32 film_x = -1.0f + 2.0f*((f32)x / (f32) WIDTH);
            v3 film_p = v3_add(film_c, v3_add(v3_scalar_mul(camera_x, half_film_width*film_x), v3_scalar_mul(camera_y, half_film_heigth*film_y)));

            v3 ray_origin = camera_p;
            v3 ray_direction = NOZ(v3_sub(film_p, camera_p));

            v3 color = ray_cast(&world, ray_origin, ray_direction);

            u32 packed_color = pack_color(color);

            IMAGE[y][x] = packed_color;
        }
        
    }

    save_to_ppm("output.ppm");
    printf("Hello Someone\n");
    return(0);
}
