#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#include <Windows.h>
#include <Xinput.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#endif

#endif // PLATFORM_H
