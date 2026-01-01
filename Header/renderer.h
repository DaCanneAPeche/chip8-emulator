#ifndef RENDERER_H_BKGBJG2Q
#define RENDERER_H_BKGBJG2Q

#include <SDL2/SDL.h>
#include "vector.h"
#include <stdint.h>

typedef struct Pixel
{
  uint8_t r, g, b;
} Pixel;

const Pixel offColor = {0, 0, 0};
const Pixel onColor = {200, 200, 200};

typedef struct Renderer {
	SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
  SDL_Texture* texture;
  Pixel* framebuffer;
	char* title;
	Vec2i size;
} Renderer;

void getRGBMasks(int* rmask, int* gmask, int* bmask)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  *rmask = 0x00ff0000;
  *gmask = 0x0000ff00;
  *bmask = 0x000000ff;
#else
  // Flip bytes order
  *rmask = 0x000000ff;
  *gmask = 0x0000ff00;
  *bmask = 0x00ff0000;
#endif
}

struct Renderer initRenderer(char* windowTitle, Vec2i windowSize)
{
	struct Renderer renderer = {NULL, NULL, NULL, NULL, NULL, windowTitle, windowSize};

	renderer.window = SDL_CreateWindow(renderer.title, 0, 0, renderer.size.x, renderer.size.y, 0);
  renderer.renderer = SDL_CreateRenderer(renderer.window, -1, SDL_RENDERER_ACCELERATED);
  renderer.framebuffer = malloc((size_t)renderer.size.x * (size_t)renderer.size.y * sizeof(Pixel));

	return renderer;
}

void setPixel(Renderer* renderer, int x, int y, Pixel color)
{
  if (x >= renderer->size.x || y >= renderer->size.y || x < 0 || y < 0) return;
  renderer->framebuffer[y * renderer->size.x + x] = color;
}

void flipPixel(Renderer* renderer, int x, int y)
{
  if (x >= renderer->size.x || y >= renderer->size.y || x < 0 || y < 0) return;
  const Pixel currentColor = renderer->framebuffer[y * renderer->size.x + x];

  // if (currentColor == offColor)
  if ((currentColor.r == offColor.r) && (currentColor.g == offColor.g) && 
      (currentColor.b == offColor.b))
    renderer->framebuffer[y * renderer->size.x + x] = onColor;
  else
    renderer->framebuffer[y * renderer->size.x + x] = offColor;
}

void clearScreen(Renderer* renderer)
{
  for (int i = 0 ; i < renderer->size.x * renderer->size.y ; i++)
  {
    renderer->framebuffer[i] = offColor;
  }
}

void destroyRenderer(Renderer* renderer)
{
  SDL_DestroyTexture(renderer->texture);
	SDL_DestroyWindow(renderer->window);
  SDL_Quit();
}

#endif /* end of include guard: RENDERER_H_BKGBJG2Q */
