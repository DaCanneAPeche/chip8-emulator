#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define FONT_STARTING_ADRESS 0x50

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

void storeDelayTimerInRegister(char Vindex, Timers* timers, Registers* registers)
{
  if (timers->delay < 0) timers->delay = 0;
  if (timers->delay > 255) timers->delay = 255;
  registers->V[Vindex] = (uint8_t)timers->delay;
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
    registers->V[0xF] = 0x01;
  else registers->V[0xF] = 0x00;

  registers->V[VX] = sum % 256;
}

void substractRegistersAndIndicateBorrow(char VX, char VY, Registers* registers)
{
  int substraction = registers->V[VX] - registers->V[VY];

  if (substraction < 0) // Borrow occures
  {
    registers->V[0xF] = 0x00;
    /* registers->V[VX] = 0; */
  }
  else
  {
    registers->V[0xF] = 0x01;
  }
    registers->V[VX] = substraction;
}

void substractRegistersTheOtherWayAndIndicateBorrow(char VX, char VY, Registers* registers)
{
  int substraction = registers->V[VY] - registers->V[VX];

  if (substraction < 0) // Borrow occures
  {
    registers->V[0xF] = 0x00;
    /* registers->V[VX] = 0; */
  }
  else
  {
    registers->V[0xF] = 0x01;
  }
    registers->V[VX] = substraction;
}

void shiftRegisterLeft(char VX, char VY, Registers* registers)
{
  registers->V[0xF] = (registers->V[VY] & 0x80) >> 7;
  registers->V[VX] = registers->V[VY] << 1;
}

void shiftRegisterRight(char VX, char VY, Registers* registers)
{
  registers->V[0xF] = registers->V[VY] & 0x01;
  registers->V[VX] = registers->V[VY] >> 1;
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

void skipIfKeyIsPressed(char Vindex, Registers* registers, int* PC)
{
  if (isKeyPressed(registers->V[Vindex])) *PC += 2;
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

void waitForInput(char Vindex, InputManager* inputManager)
{
  inputManager->VIndexToStoreInput = Vindex;  
  inputManager->waitForInput = true;
}

void storeBinaryCodedDecimal(char Vindex, char* memory, Registers* registers)
{
  uint8_t value = registers->V[Vindex];
  memory[registers->I]     = value / 100;          // hundreds digit
  memory[registers->I + 1] = (value / 10) % 10;    // tens digit
  memory[registers->I + 2] = value % 10;           // ones digit
}

void fillRegisters(char maxVindex, char* memory, Registers* registers)
{
  for (size_t i = 0 ; i <= maxVindex ; i++) // Including maxVindex (<= and not <)
  {
    registers->V[i] = memory[registers->I + i];
  }
  registers->I += maxVindex + 1;
}

void setIAsDigitAdress(char Vindex, Registers* registers)
{
  registers->I = FONT_STARTING_ADRESS + (5 * registers->V[Vindex]);
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
        case 0x5: substractRegistersAndIndicateBorrow(nibbles[1], nibbles[2], registers); break;
        case 0x6: shiftRegisterRight(nibbles[1], nibbles[2], registers); break;
        case 0x7: substractRegistersTheOtherWayAndIndicateBorrow(nibbles[1], nibbles[2], registers); break;
        case 0xE: shiftRegisterLeft(nibbles[1], nibbles[2], registers); break;
        default: return unsupported(); break;
      }
      break;

    case 0xA: storeInRegisterI(nibbles[1], nibbles[2], nibbles[3], registers); break;
    case 0xC: assignRegisterRandomValue(nibbles[1], combine2Nibbles(nibbles[2], nibbles[3]), registers); break;
    case 0xD: drawSprite(nibbles[1], nibbles[2], nibbles[3], renderer, registers, memory); break;
              
    case 0xE:
      switch (combine2Nibbles(nibbles[2], nibbles[3]))
      {
        case 0x9E: skipIfKeyIsPressed(nibbles[1], registers, PC); break;
        default: return unsupported();
      }
      break;

    case 0xF:
      switch (combine2Nibbles(nibbles[2], nibbles[3]))
      {
        case 0x07: storeDelayTimerInRegister(nibbles[1], timers, registers); break;
        case 0x0A: waitForInput(nibbles[1], inputManager); break;
        case 0x15: setDelayTimer(nibbles[1], timers, registers); break;
        case 0x18: setSoundTimer(nibbles[1], timers, registers); break;
        case 0x1E: addVxToI(nibbles[1], registers); break;
        case 0x29: setIAsDigitAdress(nibbles[1], registers); break;
        case 0x33: storeBinaryCodedDecimal(nibbles[1], memory, registers); break;
        case 0x65: fillRegisters(nibbles[1], memory, registers); break;
        default: return unsupported();
      }
      break;

    default: return unsupported(); break;
  }

  return 1;
}

#endif
