#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdint.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Texture* colorBufferTexture = NULL;
uint32_t* colorBuffer = NULL;

int window_width = 800;
int window_height = 600;


bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    // Create a SDL Window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    return true;
}

bool setup(void) {
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

    return true;
}

void process_input(void) {
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

void update(void) {
    // TODO:
}

void render_color_buffer()
{
    SDL_UpdateTexture(
        colorBufferTexture,
        NULL,
        colorBuffer,
        (window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void clear_color_buffer(uint32_t color)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            colorBuffer[(window_width * y) + x] = color;
        }
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    render_color_buffer();
    clear_color_buffer(0x0000FF00);

    SDL_RenderPresent(renderer);
}

void destroy_window()
{
    free(colorBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    return 0;
}
