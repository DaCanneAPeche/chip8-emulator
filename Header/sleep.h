#ifndef SLEEP_H
#define SLEEP_H

void sleepMS(int ms);

#if defined(__unix__) || defined(__APPLE__)

#include <unistd.h>

void sleepMS(int ms)
{
    usleep(ms * 1000);
}

#elif defined(_WIN32) || defined(_WIN64)

#include <windows.h>

void sleepMS(int ms)
{
    Sleep(sleepMs);
}

#else

#error "Unsupported platform for the sleep function !"

#endif

#endif
