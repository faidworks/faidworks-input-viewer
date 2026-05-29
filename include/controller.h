#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>
#include "platform.h"

class Controller
{
public:
    Controller();
    ~Controller();
    void update();

    struct GamepadState
    {
        uint16_t buttons;
        int16_t leftThumbX;
        int16_t leftThumbY;
        int16_t rightThumbX;
        int16_t rightThumbY;
    };

    GamepadState getState() const;

    bool isButtonPressed(int index) const;
    int16_t getAxisValue(int index) const;

    int getNewlyPressedButton() const;
    int getNewlyActiveAxis(int threshold = 16000) const;
    bool isConnected() const;

    static const char *buttonName(int index);
    static const char *axisName(int index);

private:
    void tryConnect();
#ifdef _WIN32
    XINPUT_STATE state;
    XINPUT_STATE prevState;
#else
    SDL_Gamepad *gamepad;
    SDL_Joystick *joystick;
    bool currButtons[21] = {};
    bool prevButtons[21] = {};
    int16_t currAxes[6] = {};
    int16_t prevAxes[6] = {};
#endif
};

#endif // CONTROLLER_H
