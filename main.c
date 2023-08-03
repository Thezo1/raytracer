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

f32 compute_light(World *world, v3 N, v3 P)
{
    f32 i = 0.0f;
    for(u32 light_index = 0;
            light_index < world->light_count;
            ++light_index)
    {

        Light light = world->lights[light_index];
        v3 L;

        switch(light.light_index)
        {
            case(0):
            {
                i += light.i;
            }break;

            case(1):
            {
                L = v3_add(light.position, P);
            }break;

            case(2):
            {
                L = light.direction;
            }break;
        }

        f32 tolerance = 0.0001f;
        f32 n_dot_l = dot(N, L);
        if(n_dot_l > tolerance)
        {
            i += light.i * n_dot_l/(length(N) * length(L));
        }
    }

    return(i);
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
            f32 t = (-plane.d - dot(plane.N, ray_origin)) / denom;
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
                v3 P = v3_add(ray_origin, v3_scalar_mul(ray_direction, t1));
                v3 N = v3_sub(P, sphere.point);

                f32 L = compute_light(world, N, P);
                result = v3_scalar_mul(world->materials[sphere.mat_index].color, L);
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
    materials[0].color = V3(0.3f, 0.3f, 0.3f);
    materials[1].color = V3(0.5f, 0.5f, 0.5f);
    materials[2].color = V3(0.7f, 0.5f, 0.3f);

    Plane plane = {};
    plane.N = V3(0, 0, 1);
    plane.d = 0;
    plane.mat_index = 1;

    Sphere sphere = {};
    sphere.r = 1.0f;
    sphere.point = V3(0, 0, 0);
    sphere.mat_index = 2;

    Sphere sphere2 = {};
    sphere2.r = 1.0f;
    sphere2.point = V3(3, 5, 2);
    sphere2.mat_index = 2;

    Sphere spheres[2] = {};
    spheres[0] = sphere;
    spheres[1] = sphere2;

    // NOTE: light_index 0 for ambient, 1 for point, 2 for directional
    // TODO: Switch to a better way for storing the types of light
    Light lights[3] = {};

    Light ambient_light = {};
    ambient_light.light_index = 0;
    ambient_light.i = 0.2;

    Light point_light = {};
    point_light.light_index = 1;
    point_light.i = 0.6;
    point_light.position = V3(2.0f, 1.0f, 0.0f);

    Light directional_light = {};
    directional_light.light_index = 2;
    directional_light.i = 0.2;
    directional_light.direction = V3(1.0f, 4.0f, 4.0f);

    lights[0] = ambient_light;
    lights[1] = point_light;
    lights[2] = directional_light;

    World world = {};
    world.material_count = 1;
    world.materials = materials;
    world.plane_count = 1;
    world.planes = &plane;
    world.sphere_count = 2;
    world.spheres = spheres;
    world.light_count = 3;
    world.lights = lights;

    v3 camera_p = V3(0, -10, 1);
    v3 camera_z = NOZ(camera_p);
    v3 camera_x = NOZ(cross(camera_z, V3(0, 0, 1)));
    v3 camera_y = NOZ(cross(camera_z, camera_x));

    f32 canvas_dist = 1.0f;
    f32 canvas_width = 1.0f;
    f32 canvas_height = 1.0f;
    if(WIDTH > HEIGHT)
    {
        canvas_height = canvas_width * ((f32)HEIGHT / (f32)WIDTH);
    }
    else if(HEIGHT > WIDTH)
    {
        canvas_width = canvas_height * ((f32)WIDTH / (f32)HEIGHT);
    }

    f32 half_canvas_width = 0.5f*canvas_width;
    f32 half_canvas_heigth = 0.5f*canvas_height;
    v3 canvas_c = v3_sub(camera_p, v3_scalar_mul(camera_z, canvas_dist));

    for(int y = 0; 
            y < HEIGHT; 
            ++y)
    {
        f32 canvas_y = -1.0f + 2.0f*((f32)y / (f32)HEIGHT);
        for(int x = 0; 
                x < WIDTH; 
                ++x)
        {
            f32 canvas_x = -1.0f + 2.0f*((f32)x / (f32) WIDTH);
            v3 canvas_p = v3_add(canvas_c, v3_add(v3_scalar_mul(camera_x, half_canvas_width*canvas_x), v3_scalar_mul(camera_y, half_canvas_heigth*canvas_y)));

            v3 ray_origin = camera_p;
            v3 ray_direction = NOZ(v3_sub(canvas_p, camera_p));

            v3 color = ray_cast(&world, ray_origin, ray_direction);

            u32 packed_color = pack_color(color);

            IMAGE[y][x] = packed_color;
        }
        printf("\rRaying %d%%...    ", 100 * y / HEIGHT);
        fflush(stdout);
        
    }
    fprintf(stdout, "\nFinished\n");

    save_to_ppm("output.ppm");
    printf("Hello Someone\n");
    return(0);
}
