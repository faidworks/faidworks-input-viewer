#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "platform.h"

class Controller
{
public:
    Controller();
    ~Controller();
    void update();
    // Structure to hold common Gamepad state
    struct GamepadState
    {
        uint16_t buttons;
        int16_t leftThumbX;
        int16_t leftThumbY;
        int16_t rightThumbX;
        int16_t rightThumbY;
    };
    GamepadState getState() const;

private:
#ifdef _WIN32
    XINPUT_STATE state;
#else
    SDL_GameController *controller;
    SDL_Joystick *joystick;
#endif
};

#endif // CONTROLLER_H
