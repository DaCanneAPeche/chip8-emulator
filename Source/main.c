#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "renderer.h"
#include "vector.h"
#include "instructions.h"
#include "registers.h"
#include "timers.h"
#include "audio_manager.h"
#include "input_manager.h"
#include "sleep.h"

const int ZOOM = 12;

// in bytes
const size_t RAM_SIZE = 0xFFF;
const size_t PROGRAM_START_ADRESS = 0x200;
const size_t VARIABLE_AND_DISPLAY_RAM_SIZE = 0x160;

const size_t PROGRAM_SIZE = RAM_SIZE - VARIABLE_AND_DISPLAY_RAM_SIZE - PROGRAM_START_ADRESS;

const double SIMULATED_TIMER_SPEED_HZ = 60;
const int INSTRUCTION_TIME_MS = 1; // Time between each instruction

void loadProgram(char* path, uint8_t* program)
{
  FILE* pFile = fopen(path, "r");
  if (pFile == NULL)
  {
    printf("Unable to open file");
  }
  fread(program, 1, PROGRAM_SIZE, pFile);
  fclose(pFile);
}

void printMemory(uint8_t* memory, size_t length)
{
  for (size_t i = 0 ; i < length ; i++)
  {
    printf("%u:0x%02X ", (unsigned int)(i), memory[i]);
  }

  printf("\n");
}


double getTimeInMilliseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);  // monotonic clock
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1.0e6;  // ms
}

void clearRegisters(Registers* registers)
{
  for (int i = 0 ; i < 16 ; i++)
  {
    registers->V[i] = 0;
  }
  registers->I = 0;
}

void fillFontData(uint8_t* memory, uint16_t startingAdress)
{
  uint8_t fontSet[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  memcpy(memory + startingAdress, fontSet, 80);
}

void run(Renderer* renderer, AudioManager* audioManager)
{
  srand(time(NULL));

  bool running = true;
  uint8_t* memory = malloc(RAM_SIZE);
  fillFontData(memory, 0x50);
  
  int PC = PROGRAM_START_ADRESS;
  Registers registers;
  clearRegisters(&registers);
  Timers timers = {0, 0};
  InputManager inputManager = {false};
  
  loadProgram("./exemple_programs/chipquarium/chipquarium.ch8", memory + PROGRAM_START_ADRESS);
  Debugger debugger = {false, 0, &PC, &registers, &timers, &inputManager};

  clearScreen(renderer);
  SDL_Rect sourceRect = {0, 0, 64, 32};
  SDL_Rect upscaledRect = {0, 0, 64 * ZOOM, 32 * ZOOM};

  double formerTime = getTimeInMilliseconds() / 1000;

  while (running)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT || PC > RAM_SIZE)
      {
        running = false;
      }

      else if (event.type == SDL_KEYDOWN)
      {
        bool isKeyValid = true;
        bool ubfyirbhrfed = true;
        uint8_t key = convertInputSym(event.key.keysym.sym, &isKeyValid);

        /* printf("%i : %i\n", event.key.keysym.scancode, */
        /*     convertInputScanCode(event.key.keysym.scancode, &ubfyirbhrfed)); */

        if (isKeyValid && inputManager.waitForInput)
        {
          registers.V[inputManager.VIndexToStoreInput] = key;
          inputManager.waitForInput = false;
        } 

        // Toogle debugger rendering when the space key is pressed
        if (event.key.keysym.sym == SDLK_SPACE) debugger.render = !debugger.render;
      }
    }
    if (!running) break;

    if (!inputManager.waitForInput)
    {
      unsigned short instruction = readNextInstruction(&PC, memory);
      debugger.instruction = instruction;
      printf("%i : 0x%04X\n", PC, instruction); 
      int result = interpretInstuction(instruction, renderer, &registers, &timers,
          &inputManager, memory, &PC);
      if (result == -1) running = false;
    }

    double newTime = getTimeInMilliseconds();
    double elapsedTime = newTime - formerTime;
    double simulatedClockElapsedTime = elapsedTime/1000 * SIMULATED_TIMER_SPEED_HZ;
    formerTime = newTime;

    if (timers.sound > 0)
    {
      // Before the sound is emitted since a sound timer set to 0x1 is ignored
      timers.sound -= simulatedClockElapsedTime;
      audioManager->playSound = true;
    } else
    {
      timers.sound = 0;
      audioManager->playSound = false;
    }

    if (timers.delay > 0) timers.delay -= simulatedClockElapsedTime;
    else timers.delay = 0;

    int rmask, gmask, bmask;
    getRGBMasks(&rmask, &bmask, &gmask);

    renderer->surface = SDL_CreateRGBSurfaceFrom(renderer->framebuffer,
        renderer->size.x, renderer->size.y, 24, 3 * renderer->size.x, rmask, gmask, bmask, 0);

    SDL_RenderClear(renderer->renderer);
    renderer->texture = SDL_CreateTextureFromSurface(renderer->renderer, renderer->surface);
    SDL_FreeSurface(renderer->surface);

    SDL_RenderCopy(renderer->renderer, renderer->texture, &sourceRect, &upscaledRect);
    renderDebugger(&debugger, renderer);
    SDL_RenderPresent(renderer->renderer);
    sleepMS(INSTRUCTION_TIME_MS);
  }
}

int main(int argc, char** argv)
{
  printf("CHIP-8 emulator\n");

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL image could not be initialized!\n"
        "SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  if(SDL_Init(SDL_INIT_AUDIO) < 0)
  {
    printf("SDL audio could not be initialized!\n"
        "SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  Vec2i size = {64, 32};
  Renderer renderer = initRenderer("Hello, world !", size, ZOOM);
  AudioManager audioManager;
  initAudioManager(&audioManager);
    
  run(&renderer, &audioManager);
  destroyRenderer(&renderer);
}
