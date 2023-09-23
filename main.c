#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include "./ray.h"
#include "./ray_math.h"

#define WIDTH 1280
#define HEIGHT 720
#define SAMPLES_PER_PIXEL 100

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

int main(int argc, char *argv[])
{

    v3 color1 = V3(0.9, 0.6, 0.75);
    v3 color2 = V3(0.7, 0.1, 0.25);
    v3 color3 = v3_mul(color1, color2);

    u32 other_color = pack_color_big(color1);
    u32 color = pack_color_big(color3);

    v4 center = Point(0., 0., 0.);
    f32 radius = 80.0;

    f32 upper = center.y + radius;

    v4 upper_point = Point(0.0, upper, 0.0);

    v4 pos = upper_point;

    for(int i = 0;
            i < 12;
            ++i)
    {
        m4x4 inv_o_clock_matrix = {};
        bool can_invert = m4x4_invert(m4x4_rotateZ_matrix(PI32/6.), &inv_o_clock_matrix);

        if(can_invert)
        {
            v4 o_clock = v4_transform(inv_o_clock_matrix, pos);

            int canvas_x = ((f32)WIDTH/2) + o_clock.x;
            int canvas_y = ((f32)HEIGHT/2) - o_clock.y;

            IMAGE[canvas_y][canvas_x] = other_color;

            pos = o_clock;
        }
    }

    save_to_ppm("output.ppm");
    return(0);
}
