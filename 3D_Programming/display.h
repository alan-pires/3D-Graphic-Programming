#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdint.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS) // tempo em milisec que precisa esperar para atingir o FPS desejado

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* colorBufferTexture;
extern uint32_t* colorBuffer;
extern int window_width;
extern int window_height;

bool initialize_window(void);
void draw_grid();
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_rectangle(int x, int y, int width, int height, uint32_t color);
void render_color_buffer();
void clear_color_buffer(uint32_t color);
void destroy_window();

#endif