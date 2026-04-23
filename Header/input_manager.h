#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdbool.h>
#include "SDL2/SDL.h"

typedef struct InputManager
{
  bool waitForInput;
  char VIndexToStoreInput;
} InputManager;

// Scancode -> input
uint8_t scanCodeInputMap[] = {
  27, 0, // 0
  30, 1, // 1
  31, 2, // 2
  32, 3, // 3
  20, 4, // 4
  26, 5, // 5
  8,  6, // 6
  4,  7, // 7
  22, 8, // 8
  7,  9, // 9

  29, 0xa, // A
  6,  0xb, // B
  33, 0xc, // C
  21, 0xd, // D
  9,  0xe, // E
  25, 0xf  // F
};

/* Other possible input map : keyName -> gameKey (pressing A on the keyboard gives A in game)
uint8_t keyCodeInputMap[] = {
  SDLK_0, 0,
  SDLK_1, 1,
  SDLK_2, 2,
  SDLK_3, 3,
  SDLK_4, 4,
  SDLK_5, 5,
  SDLK_6, 6,
  SDLK_7, 7,
  SDLK_8, 8,
  SDLK_9, 9,

  SDLK_a, 0xa,
  SDLK_b, 0xb,
  SDLK_c, 0xc,
  SDLK_d, 0xd,
  SDLK_e, 0xe,
  SDLK_f, 0xf
};
*/

uint8_t convertKeyboardInputToGame(SDL_Scancode keyCode, bool* isInputValid)
{
  *isInputValid = true;
  for (size_t i = 0 ; i < 16 ; i++) // 16 different outputs
    if (keyCode == scanCodeInputMap[2*i]) return scanCodeInputMap[2*i + 1];

  *isInputValid = false;
  return 0;
}

SDL_Scancode convertGameInputToKeyboard(uint8_t key, bool* isInputValid)
{
  *isInputValid = true;
  for (size_t i = 0 ; i < 16 ; i++) // 16 different outputs
    if (key == scanCodeInputMap[2*i + 1]) return scanCodeInputMap[2*i];

  *isInputValid = false;
  return 0;
}

bool isKeyPressed(uint8_t key)
{
  bool isInputValid = true;
  SDL_Scancode keyCode = convertGameInputToKeyboard(key, &isInputValid);
  const uint8_t* keyboardState = SDL_GetKeyboardState(NULL);

  return isInputValid && keyboardState[keyCode];
}

#endif
