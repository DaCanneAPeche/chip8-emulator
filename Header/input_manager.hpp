#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <stdbool.h>
#include "SDL2/SDL.h"

typedef struct InputManager
{
  bool waitForInput;
} InputManager;

uint8_t convertInputSym(SDL_KeyCode keyCode, bool* isInputValid)
{
  switch (keyCode)
  {
    case SDLK_0: return 0;
    case SDLK_1: return 1;
    case SDLK_2: return 2;
    case SDLK_3: return 3;
    case SDLK_4: return 4;
    case SDLK_5: return 5;
    case SDLK_6: return 6;
    case SDLK_7: return 7;
    case SDLK_8: return 8;
    case SDLK_9: return 9;

    case SDLK_a: return 0xa;
    case SDLK_b: return 0xb;
    case SDLK_c: return 0xc;
    case SDLK_d: return 0xd;
    case SDLK_e: return 0xe;
    case SDLK_f: return 0xf;

    default: *isInputValid = false;
  }
  return 0;
}

#endif
