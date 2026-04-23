#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdbool.h>
#include "SDL2/SDL.h"

typedef struct InputManager
{
  bool waitForInput;
  char VIndexToStoreInput;
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

uint8_t convertInputScanCode(SDL_Scancode keyCode, bool* isInputValid)
{
  switch (keyCode)
  {
    case 88: return 0;
    case 49: return 1;
    case 50: return 2;
    case 51: return 3;
    case 81: return 4;
    case 87: return 5;
    case 69: return 6;
    case 65: return 7;
    case 83: return 8;
    case 68: return 9;

    case 90: return 0xa;
    case 67: return 0xb;
    case 52: return 0xc;
    case 82: return 0xd;
    case 70: return 0xe;
    case 86: return 0xf;

    default: *isInputValid = false;
  }
  return 0;
}

SDL_KeyCode convertInputSymToSDL(uint8_t key, bool* isInputValid)
{
  switch (key)
  {
    case 0: return SDLK_0;
    case 1: return SDLK_1;
    case 2: return SDLK_2;
    case 3: return SDLK_3;
    case 4: return SDLK_4;
    case 5: return SDLK_5;
    case 6: return SDLK_6;
    case 7: return SDLK_7;
    case 8: return SDLK_8;
    case 9: return SDLK_9;

    case 0xa: return SDLK_a;
    case 0xb: return SDLK_b;
    case 0xc: return SDLK_c;
    case 0xd: return SDLK_d;
    case 0xe: return SDLK_e;
    case 0xf: return SDLK_f;

    default: *isInputValid = false;
  }
  return 0;
}

bool isKeyPressed(uint8_t key)
{
  bool isInputValid = true;
  SDL_KeyCode keyCode = convertInputSymToSDL(key, &isInputValid);
  const uint8_t* keyboardState = SDL_GetKeyboardState(NULL);

  return isInputValid && keyboardState[SDL_GetScancodeFromKey(keyCode)];
}

#endif
