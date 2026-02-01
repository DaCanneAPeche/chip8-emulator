#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
#include <math.h>

#include "renderer.h"
#include "registers.h"
#include "subroutines.h"
#include "timers.h"
#include "input_manager.hpp"

unsigned char combine2Nibbles(char first, char second)
{
  return (unsigned char)first << 4 | (unsigned char)second;
}

unsigned short combine3Nibbles(char first, char second, char third)
{
  return (unsigned short)first << 8 | (unsigned short)second << 4 |
         (unsigned short)third;
}

int unsupported()
{
  printf("Unsupported instruction\n");
  return -1;
}

void setSoundTimer(char Vindex, Timers* timers, Registers* registers)
{
  timers->sound = (uint8_t)registers->V[Vindex];
}

void setDelayTimer(char Vindex, Timers* timers, Registers* registers)
{
  timers->delay = (uint8_t)registers->V[Vindex];
}

void storeInRegisterV(char Vindex, char firstNibble, char secondNibble, Registers* registers)
{
  registers->V[Vindex] = combine2Nibbles(firstNibble, secondNibble);
}

void storeInRegisterI(char firstNibble, char secondNibble, char thirdNibble, Registers* registers)
{
  registers->I = combine3Nibbles(firstNibble, secondNibble, thirdNibble);
}

void addToRegisterV(char Vindex, unsigned char amount, Registers* registers)
{
  unsigned int sum = amount + registers->V[Vindex];
  registers->V[Vindex] = sum % 256;
}

void sumRegistersAndIndicateCarry(char VX, char VY, Registers* registers)
{
  unsigned int sum = registers->V[VX] + registers->V[VY];

  if (sum > 255) // Carry occures
    registers->V[15] = 0x01;
  else registers->V[15] = 0x00;

  registers->V[VX] = sum % 256;
}

void addVxToI(char Vindex, Registers* registers)
{
  registers->I += registers->V[Vindex];
}

void drawSprite(int vx, int vy, int byteLength, Renderer* renderer,
    Registers* registers, char* memory)
{
  unsigned short spriteAdress = registers->I;
  uint8_t x = registers->V[vx];
  uint8_t y = registers->V[vy];
  // printf("%u\n", x);

  // printf("0x%04X\n", registers->I); 
  // printf("%i\n", spriteAdress);
  for (int i = 0 ; i < byteLength ; i++)
  {
    unsigned char row = (unsigned char)memory[spriteAdress + i];
    // printf("%i:%u\n", i + spriteAdress, row);

    for (int j = 0 ; j < 8 ; j++)
    {
      if (row & (unsigned int)pow(2, 7 - j))
      {
        // printf("j: %i\n", j);
        flipPixel(renderer, x + j, y + i);
      }
    }

  }
  // printf("\n");
}

void skipIfVEquals(char Vindex, char value, Registers* registers, int* PC)
{
  if (registers->V[Vindex] == value) *PC += 2;
}

void skipIfVNotEquals(char Vindex, char value, Registers* registers, int* PC)
{
  if (registers->V[Vindex] != value) *PC += 2;
}

void setVXasVY(char VX, char VY, Registers* registers)
{
  registers->V[VX] = registers->V[VY];
}

void setVXasVY_AND(char VX, char VY, Registers* registers)
{
  registers->V[VX] = registers->V[VY] & registers->V[VX];
}

void assignRegisterRandomValue(char Vindex, int16_t mask, Registers* registers)
{
  int16_t randomNumber = rand() % 0xFF;
  registers->V[Vindex] = randomNumber & mask;
}

unsigned short readNextInstruction(int* PC, char* program)
{
  unsigned short instruction = ((unsigned char)program[*PC] << 8) |
                                (unsigned char)program[*PC + 1];
  *PC += 2;
  return instruction;
}

void jumpTo(int index, int* PC)
{
  *PC = index;
}

int newSubroutine(int* PC, int adress)
{
  int previousPC = *PC; // PC is incremented before the instruction is executed
  *PC = adress;
  return pushSubroutine(previousPC); // success is returned as 1, failure as -1
}

void endOFSubroutine(int* PC)
{
  *PC = popSubroutine();
}

void waitForInput(InputManager* inputManager)
{
  inputManager->waitForInput = true;
}

// Extract hexadecimal "figures"
void extractNibbles(char nibbles[4], unsigned short opcode)
{
  nibbles[0] = (opcode >> 12) & 0xF; 
  nibbles[1] = (opcode >>  8) & 0xF; 
  nibbles[2] = (opcode >>  4) & 0xF; 
  nibbles[3] = (opcode      ) & 0xF; 
}

int interpretInstuction(unsigned short instruction, Renderer* renderer,
    Registers* registers, Timers* timers, InputManager* inputManager,
    char* memory, int* PC)
{
  char nibbles[4];
  extractNibbles(nibbles, instruction);

  switch (nibbles[0])
  {
    case 0x0:
      switch (instruction)
      {
        case 0x00E0: clearScreen(renderer); break;
        case 0x00EE: endOFSubroutine(PC); break;
        default: return unsupported(); break;
      }
      break;

    case 0x1: jumpTo(combine3Nibbles(nibbles[1], nibbles[2], nibbles[3]), PC); break;
    case 0x2: return newSubroutine(PC, combine3Nibbles(nibbles[1], nibbles[2], nibbles[3])); break;
    case 0x3: skipIfVEquals(nibbles[1], combine2Nibbles(nibbles[2], nibbles[3]), registers, PC); break; 
    case 0x4: skipIfVNotEquals(nibbles[1], combine2Nibbles(nibbles[2], nibbles[3]), registers, PC); break; 
    case 0x6: storeInRegisterV(nibbles[1], nibbles[2], nibbles[3], registers); break;
    case 0x7: addToRegisterV(nibbles[1], combine2Nibbles(nibbles[2], nibbles[3]), registers); break;

    case 0x8:
      switch (nibbles[3])
      {
        case 0x0: setVXasVY(nibbles[1], nibbles[2], registers); break;
        case 0x2: setVXasVY_AND(nibbles[1], nibbles[2], registers); break;
        case 0x4: sumRegistersAndIndicateCarry(nibbles[1], nibbles[2], registers); break;
        default: return unsupported(); break;
      }
      break;

    case 0xA: storeInRegisterI(nibbles[1], nibbles[2], nibbles[3], registers); break;
    case 0xC: assignRegisterRandomValue(nibbles[1], combine2Nibbles(nibbles[2], nibbles[3]), registers); break;
    case 0xD: drawSprite(nibbles[1], nibbles[2], nibbles[3], renderer, registers, memory); break;

    case 0xF:
      switch (combine2Nibbles(nibbles[2], nibbles[3]))
      {
        case 0x0A: waitForInput(inputManager); break;
        case 0x15: setDelayTimer(nibbles[1], timers, registers); break;
        case 0x18: setSoundTimer(nibbles[1], timers, registers); break;
        case 0x1E: addVxToI(nibbles[1], registers); break;
        default: return unsupported();
      }
      break;

    default: return unsupported(); break;
  }

  return 1;
}

#endif
