#include <stdio.h>
#include "display.h"
#include "vector.h"

bool is_running = false;
#define N_POINTS (9 * 9 * 9)
vec3_t glb_cubePoints[N_POINTS];
vec2_t glb_projected_points[N_POINTS];
float glb_fov = 128;

bool setup(void)
{
    colorBuffer = (uint32_t*)malloc(window_width * window_height * sizeof(uint32_t));
    if (!colorBuffer)
    {
        fprintf(stderr, "Error allocating colorBuffer memory\n");
        return false;
    }

    //Creating a SDL texture that is used to display the color buffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    int point_count = 0;

    //Loads array of vectors
    for (float x = -1; x <= 1; x += 0.25)
    {
        for (float y = -1; y <= 1; y += 0.25)
        {
            for (float z = -1; z <= 1; z += 0.25)
            {
                vec3_t new_point = { x, y, z };
                glb_cubePoints[point_count++] = new_point;
            }
        }
    }

    return true;
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;
        break;
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        point.x * glb_fov,
        point.y * glb_fov
    };
    return projected_point;
}

void update(void)
{
    for (int i = 0; i < N_POINTS; i++)
    {
        vec3_t point = glb_cubePoints[i];
        vec2_t projected_point = project(point);
        glb_projected_points[i] = projected_point;
    }
}

void render(void)
{
    //draw_grid();

    for (int i = 0; i < N_POINTS; i++)
    {
        vec2_t projected_point = glb_projected_points[i];
        draw_rectangle(
            projected_point.x + (window_width / 2),
            projected_point.y + (window_height / 2),
            4, 
            4, 
            0x00FFFF00
        );
    }

    render_color_buffer();
    clear_color_buffer(0x00000000);

    SDL_RenderPresent(renderer);
}

int main(void)
{
    is_running = initialize_window();

    setup();

    vec3_t myvec = { 1.0, 3.0, -4.0 };

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    return 0;
}