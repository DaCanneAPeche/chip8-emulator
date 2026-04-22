#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int PC;
  void* previous;
  void* next;
} Subroutine;


Subroutine baseSubroutine = {200, NULL, NULL};

Subroutine* findLastSubroutine()
{
  Subroutine* lastSubroutine = &baseSubroutine;
  while (lastSubroutine->next != NULL)
  {
    lastSubroutine = (Subroutine*)lastSubroutine->next;
  }
  return lastSubroutine;
}

void pushSubroutine(int PC)
{
  Subroutine* newSubroutine = (Subroutine*)malloc(sizeof(Subroutine));
  newSubroutine->PC = PC;
  newSubroutine->next = NULL;

  Subroutine* parentSubroutine = findLastSubroutine();
  parentSubroutine->next = (void*)newSubroutine;
  newSubroutine->previous = (void*)parentSubroutine;
}

int popSubroutine()
{
  Subroutine* lastSubroutine = findLastSubroutine();
  int PC = lastSubroutine->PC;

  if (lastSubroutine == &baseSubroutine)
    printf("Cannot pop empty subroutine stack, program's PC will be set to 200\n");

  else
  {
    ((Subroutine*)lastSubroutine->previous)->next = NULL;
    free(lastSubroutine);
  }

  return PC;
}

#endif
