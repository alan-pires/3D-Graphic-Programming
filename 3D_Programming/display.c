#include "display.h"

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

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    //window_width = display_mode.w;
    //window_height = display_mode.h;

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
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void draw_pixel(int x, int y, uint32_t color)
{
    if (x > 0 && x < window_width && y > 0 && y < window_height)
        colorBuffer[(window_width * y) + x] = color;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    int deltaX = (x1 - x0);
    int deltaY = (y1 - y0);

    int longestSideLenght = (abs(deltaX) >= abs(deltaY)) ? abs(deltaX) : abs(deltaY);

    float incrX = deltaX / (float)longestSideLenght;
    float incrY = deltaY / (float)longestSideLenght;

    float currentX = x0;
    float currentY = y0;

    for (int i = 0; i <= longestSideLenght; i++)
    {
        draw_pixel(round(currentX), round(currentY), color);
        currentX += incrX;
        currentY += incrY;
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_grid()
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            if (x % 10 == 0 || y % 10 == 0)
                colorBuffer[(window_width * y) + x] = 0xFFFFFFFF;
        }
    }
}

void draw_rectangle(int x, int y, int width, int height, uint32_t color)
{
    for (int i = y; i < (height + y); i++)
    {
        for (int j = x; j < (width + x); j++)
        {
            draw_pixel(j, i, color);
        }
    }
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
    int bufferSize = window_width * window_height;

    for (int i = 0; i < bufferSize; i++)
    {
        *(colorBuffer + i) = color;
    }
}

void destroy_window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
