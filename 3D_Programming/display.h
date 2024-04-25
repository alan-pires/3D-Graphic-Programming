#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdint.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* colorBufferTexture;
extern uint32_t* colorBuffer;
extern int window_width;
extern int window_height;

bool initialize_window(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_grid();
void draw_rectangle(int x, int y, int width, int height, uint32_t color);
void render_color_buffer();
void clear_color_buffer(uint32_t color);
void destroy_window();

#endif