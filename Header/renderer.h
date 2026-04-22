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

// Return true if the pixel was set to off
bool flipPixel(Renderer* renderer, int x, int y)
{
  if (x >= renderer->size.x || y >= renderer->size.y || x < 0 || y < 0) return false;

  const Pixel currentColor = renderer->framebuffer[y * renderer->size.x + x];

  // if (currentColor == offColor)
  if ((currentColor.r == offColor.r) && (currentColor.g == offColor.g) && 
      (currentColor.b == offColor.b))
  {
    renderer->framebuffer[y * renderer->size.x + x] = onColor;
    return false;
  }
  else
  {
    renderer->framebuffer[y * renderer->size.x + x] = offColor;
    return true;
  }
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

void renderVRegisters(Debugger* debugger, Renderer* renderer, SDL_Color fg)
{
  for (int i = 0 ; i < 16 ; i++)
  {
    char* VRegisterText = (char*)malloc(sizeof(char) * 10);
    sprintf(VRegisterText, "%i : 0x%02X", i, debugger->registers->V[i]);
    /* sprintf(VRegisterText, "%i : %u", i, debugger->registers->V[i]); */
    renderText(renderer, VRegisterText, 10 + 100 * (i / 4), 30 + 20 * (i % 4), fg, 1);
    free(VRegisterText);
  }
}

void renderDebugger(Debugger* debugger, Renderer* renderer)
{
  if (!debugger->render) return;

  SDL_Color fg = {255, 0, 0, 255}; // red
  renderText(renderer, "V Registers : ", 0, 0, fg, 1);
  renderVRegisters(debugger, renderer, fg);
  
  // Repetitive code but implementing a general function is more trouble than anything
  // because of the different possibnle types of value
  char* IRegisterText = (char*)malloc(sizeof(char) * 20);
  sprintf(IRegisterText, "I Register : 0x%04X", debugger->registers->I);
  renderText(renderer, IRegisterText, 0, 120, fg, 1);
  free(IRegisterText);

  char* pointerCounterText = (char*)malloc(sizeof(char) * 20);
  sprintf(pointerCounterText, "PC : %i", *debugger->PC);
  renderText(renderer, pointerCounterText, 0, 150, fg, 1);
  free(pointerCounterText);

  char* lastInstructionText = (char*)malloc(sizeof(char) * 30);
  sprintf(lastInstructionText, "Last instruction : 0x%04X", debugger->instruction);
  renderText(renderer, lastInstructionText, 0, 180, fg, 1);
  free(lastInstructionText);

  char* delayTimerText = (char*)malloc(sizeof(char) * 30);
  sprintf(delayTimerText, "Delay timer : %f", debugger->timers->delay);
  renderText(renderer, delayTimerText, 0, 210, fg, 1);
  free(delayTimerText);

  char* soundTimerText = (char*)malloc(sizeof(char) * 30);
  sprintf(soundTimerText, "Sound timer : %f", debugger->timers->sound);
  renderText(renderer, soundTimerText, 0, 240, fg, 1);
  free(soundTimerText);

  if (debugger->inputManager->waitForInput)
    renderText(renderer, "Waiting for input", 0, 270, fg, 1);
  else
    renderText(renderer, "Not waiting for input", 0, 270, fg, 1);

}

#endif /* end of include guard: RENDERER_H_BKGBJG2Q */
