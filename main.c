#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include "./ray.h"
#include "./ray_math.h"
#include<SDL2/SDL.h>

// #define ASPECT_RATIO 16.0f/9.0f
#define WIDTH 1280
#define HEIGHT 720

static u32 IMAGE[HEIGHT][WIDTH];

void save_to_ppm(const char *filename)
{
    FILE *file;
    file = fopen(filename, "wb");
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
    v3 result = V3(0.0f, 0.0f, 0.0f);
    f32 hit_distance = F32MAX;

    f32 a = 0.5f * (ray_direction.y + 1.0f);
    v3 light_direction = NOZ(V3(-1.0f, -1.0f, -1.0f));
    // result = v3_add(v3_scalar_mul(V3(1.0f, 1.0f, 1.0f), (1.0f - a)), v3_scalar_mul(V3(0.5f, 0.7f, 1.0f), a));

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

        if(discriminant > 0.0f)
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
                v3 hit_point = v3_add(ray_origin, v3_scalar_mul(ray_direction, t));
                v3 N = NOZ(v3_sub(hit_point, sphere.point));
                hit_distance = t;

                f32 d = dot(N, v3_neg(light_direction));
                if(d < 0.0f)
                {
                    d = 0.0f;
                }

                // v3 normal_color = v3_scalar_mul(V3((N.x + 1.0f), (N.y + 1.0f), (N.z + 1.0f)), 0.5f);
                v3 sphere_color = v3_scalar_mul(world->materials[sphere.mat_index].color, d);
                result = sphere_color;
            }
        }

    }
    for(u32 plane_index = 0;
            plane_index < world->plane_count;
            ++plane_index)
    {
        Plane plane = world->planes[plane_index];
        f32 denom = dot(plane.N, ray_direction);
        if((denom < -0.001) > (denom > 0.001))
        {
            f32 t = (-plane.d - dot(plane.N, ray_origin)) / denom;
            if((t > 0) && (t < hit_distance))
            {
                hit_distance = t;
                f32 d = dot(plane.N, v3_neg(light_direction));
                if(d < 0.0f)
                {
                    d = 0.0f;
                }

                v3 plane_color = world->materials[plane.mat_index].color;
                result = v3_scalar_mul(plane_color, d);
            }
        }
    }

    return(result);
}

int main()
{
    Material materials[2] = {};
    materials[0].color = V3(0.7f, 0.5f, 0.3f);
    materials[0].specular = -1.0f;

    materials[1].color = V3(0.3f, 0.2f, 0.3f);
    materials[1].specular = -1.0f;

    
    Sphere sphere = {};
    sphere.r = 1.0f;
    sphere.point = V3(0, 0, -5);
    sphere.mat_index = 0;

    Plane plane = {};
    plane.N = V3(0.0f, 2.0f, 0.0f);
    plane.d = 1.0f;
    plane.mat_index = 1;

    World world = {};
    world.materials = materials;
    world.material_count = 2;
    world.sphere_count = 1;
    world.spheres = &sphere;
    world.plane_count = 1;
    world.planes = &plane;

    f32 viewport_dist = 1.0f;
    f32 viewport_height = 2.0f;
    f32 viewport_width = 2.0f;
    if(WIDTH > HEIGHT)
    {
        viewport_height = viewport_width * ((f32)HEIGHT/(f32)WIDTH);
    }

    if(HEIGHT > WIDTH)
    {
        viewport_width = viewport_height * ((f32)WIDTH/(f32)HEIGHT);
    }

    v3 viewport_u = V3(viewport_width, 0.0f, 0.0f);
    v3 viewport_v = V3(0.0f, -viewport_height, 0.0f);

    v3 delta_u = v3_scalar_div(viewport_u, (f32)WIDTH);
    v3 delta_v = v3_scalar_div(viewport_v, (f32)HEIGHT);

    v3 camera_position = V3(0.0f, 0.0f, 0.0f);
    v3 camera_to_viewport = v3_sub(camera_position, V3(0.0f, 0.0f, viewport_dist));
    v3 viewport_upper_left = v3_sub(v3_sub(camera_to_viewport, v3_scalar_div(viewport_u, 2.0f)), v3_scalar_div(viewport_v, 2.0f));
    v3 origin_pixel = v3_add(viewport_upper_left, v3_scalar_mul(v3_add(delta_u, delta_v), 0.5f));

    for(int y = 0; 
            y < HEIGHT; 
            ++y)
    {
        for(int x = 0; 
                x < WIDTH; 
                ++x)
        {

            v3 pixel_center = v3_add(origin_pixel, v3_add(v3_scalar_mul(delta_u, x), v3_scalar_mul(delta_v, y)));

            v3 ray_origin = camera_position;
            v3 ray_direction = NOZ(v3_sub(pixel_center, ray_origin));

            v3 packed_color = ray_cast(&world, ray_origin, ray_direction);

            u32 color = pack_color_be(packed_color);

            IMAGE[y][x] = color;

        }
        printf("\rRaying %d%%...    ", 100 * y / HEIGHT);
    }

    printf("Done...\n");
    save_to_ppm("output.ppm");

    return(0);
}
