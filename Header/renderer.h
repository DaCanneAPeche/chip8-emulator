#ifndef RENDERER_H_BKGBJG2Q
#define RENDERER_H_BKGBJG2Q

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "vector.h"
#include <stdint.h>
#include "debugger.h"

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
  TTF_Font* font;
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

struct Renderer initRenderer(char* windowTitle, Vec2i renderingSize, int zoom)
{
	struct Renderer renderer = {NULL, NULL, NULL, NULL, NULL, NULL, windowTitle, renderingSize};
	renderer.window = SDL_CreateWindow(renderer.title, 0, 0, renderer.size.x * zoom, renderer.size.y * zoom, 0);
  renderer.renderer = SDL_CreateRenderer(renderer.window, -1, SDL_RENDERER_ACCELERATED);
  renderer.framebuffer = malloc((size_t)renderer.size.x * (size_t)renderer.size.y * sizeof(Pixel));

  if (TTF_Init() < 0) {
    printf("Couldn't initialize TTF: %s\n",SDL_GetError());
  }

  renderer.font = TTF_OpenFont("assets/FiraSans-Regular.ttf", 18);
  TTF_SetFontStyle(renderer.font, TTF_STYLE_NORMAL);
  TTF_SetFontOutline(renderer.font, 0);
  TTF_SetFontKerning(renderer.font, 1);
  TTF_SetFontHinting(renderer.font, TTF_HINTING_NORMAL);

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
  TTF_CloseFont(renderer->font);
  TTF_Quit();
  SDL_Quit();
}

void renderText(Renderer* renderer, const char* text, int x, int y, SDL_Color fg,
    float fontSizeMultiplier)
{
  SDL_Surface* textSurface = TTF_RenderText_Solid(renderer->font, text, fg);
  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer->renderer, textSurface);
  SDL_FreeSurface(textSurface);

  // Native font size
  SDL_Rect renderRect = {x, y, 0, 0};
  TTF_SizeText(renderer->font, text, &renderRect.w, &renderRect.h);
  renderRect.w *= fontSizeMultiplier;
  renderRect.h *= fontSizeMultiplier;

  SDL_RenderCopy(renderer->renderer, textTexture, NULL, &renderRect);
  SDL_DestroyTexture(textTexture);
}

void renderDebugger(Debugger* debugger, Renderer* renderer)
{
  SDL_Color fg = {255, 0, 0, 255};
  renderText(renderer, "Hello, world !", 10, 10, fg, 3);
}

#endif /* end of include guard: RENDERER_H_BKGBJG2Q */
