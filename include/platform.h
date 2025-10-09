#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#include <Windows.h>
#include <Xinput.h>
#else
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>
#endif

#endif // PLATFORM_H
