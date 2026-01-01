#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <stdio.h>

#define SUBROUTINE_STACK_SIZE 16 // arbitrary number, original stack was 12
                                 // TODO : Could be set as a type of linked list

int subroutineStack[SUBROUTINE_STACK_SIZE]; 
int subroutineStackIndex = -1;

int pushSubroutine(int PC)
{
  subroutineStackIndex++;
  
  if (subroutineStackIndex >= SUBROUTINE_STACK_SIZE)
  {
    printf("Cannot push subroutine, limit is %i\n", SUBROUTINE_STACK_SIZE);
    return -1;
  }

  subroutineStack[subroutineStackIndex] = PC;

  return 1;
}

int popSubroutine()
{
  if (subroutineStackIndex <= -1)
  {
    printf("Cannot pop empty subroutine stack, program's PC will be set to 200\n");
    return 200;
  }
  int previousIndex = subroutineStackIndex;
  subroutineStackIndex--;
  return subroutineStack[previousIndex];
}

#endif
