#include <stdio.h>
#include <stdbool.h>
#include "renderer.h"
#include "vector.h"
#include "instructions.h"
#include "registers.h"
#include <unistd.h>

const int ZOOM = 7;

// in bytes
const size_t RAM_SIZE = 0xFFF;
const size_t PROGRAM_START_ADRESS = 0x200;
const size_t VARIABLE_AND_DISPLAY_RAM_SIZE = 0x160;

const size_t PROGRAM_SIZE = RAM_SIZE - VARIABLE_AND_DISPLAY_RAM_SIZE - PROGRAM_START_ADRESS;

void loadProgram(char* path, char* program)
{
  FILE* pFile = fopen(path, "r");
  if (pFile == NULL)
  {
    printf("Unable to open file");
  }
  fread(program, 1, PROGRAM_SIZE, pFile);
  fclose(pFile);
}

void printMemory(char* memory, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    printf("%u:0x%02X ", (unsigned int)(i), (unsigned char)memory[i]);
  }

  printf("\n");
}

void run(Renderer* renderer)
{
  bool running = true;
  //char memory[RAM_SIZE];
  char* memory = malloc(RAM_SIZE);
  int PC = PROGRAM_START_ADRESS;
  Registers registers;
  
  loadProgram("./exemple_programs/heartmonitor/heart_monitor.ch8", memory + PROGRAM_START_ADRESS);
  clearScreen(renderer);

  SDL_Rect sourceRect = {0, 0, 64, 32};
  SDL_Rect upscaledRect = {0, 0, 64 * ZOOM, 32 * ZOOM};

  while (running)
  {
    SDL_Event event;
    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT || PC > RAM_SIZE)
    {
      running = false;
      break;
    }

    unsigned short instruction = readNextInstruction(&PC, memory);
    printf("%i : 0x%04X\n", PC, instruction); 
    int result = interpretInstuction(instruction, renderer, &registers, memory, &PC);
    if (result == -1) running = false;

    int rmask, gmask, bmask;
    getRGBMasks(&rmask, &bmask, &gmask);

    renderer->surface = SDL_CreateRGBSurfaceFrom(renderer->framebuffer,
        renderer->size.x, renderer->size.y, 24, 3 * renderer->size.x, rmask, gmask, bmask, 0);

    SDL_RenderClear(renderer->renderer);
    renderer->texture = SDL_CreateTextureFromSurface(renderer->renderer, renderer->surface);

    SDL_RenderCopy(renderer->renderer, renderer->texture, &sourceRect, &upscaledRect);
    SDL_RenderPresent(renderer->renderer);
    usleep(1 * 1000);
  }
}

int main(int argc, char** argv)
{
  printf("CHIP-8 emulator\n");

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not be initialized!\n"
        "SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  Vec2i size = {64, 32};
  Renderer renderer = initRenderer("Hello, world !", size, ZOOM);
  run(&renderer);
  destroyRenderer(&renderer);
}
