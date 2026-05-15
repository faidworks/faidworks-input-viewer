#include "controller.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

Controller::Controller()
{
#ifdef _WIN32
    ZeroMemory(&state, sizeof(XINPUT_STATE));
    ZeroMemory(&prevState, sizeof(XINPUT_STATE));
#else
    if (!SDL_Init(SDL_INIT_GAMEPAD))
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        int count = 0;
        SDL_JoystickID *gamepads = SDL_GetGamepads(&count);
        if (gamepads && count > 0)
        {
            gamepad = SDL_OpenGamepad(gamepads[0]);
            joystick = SDL_OpenJoystick(gamepads[0]);
            if (gamepad == nullptr)
            {
                std::cerr << "Failed to open game controller! SDL Error: " << SDL_GetError() << std::endl;
            }
        }
        else
        {
            gamepad = nullptr;
            joystick = nullptr;
            std::cerr << "No gamepads found!" << std::endl;
        }
        SDL_free(gamepads);
    }
#endif
}

Controller::~Controller()
{
#ifdef _WIN32
    // No specific clean-up required for XInput
#else
    if (gamepad)
    {
        SDL_CloseGamepad(gamepad);
    }
    SDL_Quit();
#endif
}

void Controller::update()
{
#ifdef _WIN32
    prevState = state;
    XInputGetState(0, &state);
#else
    std::memcpy(prevButtons, currButtons, sizeof(currButtons));
    std::memcpy(prevAxes, currAxes, sizeof(currAxes));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
    }

    if (gamepad)
    {
        for (int i = 0; i < 21; i++)
            currButtons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);
        for (int i = 0; i < 6; i++)
            currAxes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);
    }
#endif
}

Controller::GamepadState Controller::getState() const
{
    GamepadState gs = {};
#ifdef _WIN32
    gs.buttons = state.Gamepad.wButtons;
    gs.leftThumbX = state.Gamepad.sThumbLX;
    gs.leftThumbY = state.Gamepad.sThumbLY;
    gs.rightThumbX = state.Gamepad.sThumbRX;
    gs.rightThumbY = state.Gamepad.sThumbRY;
#else
    gs.buttons = 0;
    for (int i = 0; i < 15 && i < 16; i++)
    {
        if (currButtons[i])
            gs.buttons |= (1 << i);
    }
    gs.leftThumbX = currAxes[0];
    gs.leftThumbY = currAxes[1];
    gs.rightThumbX = currAxes[2];
    gs.rightThumbY = currAxes[3];
#endif
    return gs;
}

bool Controller::isButtonPressed(int index) const
{
#ifdef _WIN32
    if (index < 0 || index >= 16)
        return false;
    return (state.Gamepad.wButtons & (1 << index)) != 0;
#else
    if (index < 0 || index >= 21)
        return false;
    return currButtons[index];
#endif
}

int16_t Controller::getAxisValue(int index) const
{
#ifdef _WIN32
    switch (index)
    {
    case 0:
        return state.Gamepad.sThumbLX;
    case 1:
        return state.Gamepad.sThumbLY;
    case 2:
        return state.Gamepad.sThumbRX;
    case 3:
        return state.Gamepad.sThumbRY;
    case 4:
        return (int16_t)(state.Gamepad.bLeftTrigger * 128);
    case 5:
        return (int16_t)(state.Gamepad.bRightTrigger * 128);
    default:
        return 0;
    }
#else
    if (index < 0 || index >= 6)
        return 0;
    return currAxes[index];
#endif
}

int Controller::getNewlyPressedButton() const
{
#ifdef _WIN32
    WORD newPresses = state.Gamepad.wButtons & ~prevState.Gamepad.wButtons;
    if (newPresses == 0)
        return -1;
    for (int i = 0; i < 16; i++)
    {
        if (newPresses & (1 << i))
            return i;
    }
    return -1;
#else
    for (int i = 0; i < 21; i++)
    {
        if (currButtons[i] && !prevButtons[i])
            return i;
    }
    return -1;
#endif
}

int Controller::getNewlyActiveAxis(int threshold) const
{
#ifdef _WIN32
    auto getAxis = [](const XINPUT_STATE &s, int i) -> int16_t
    {
        switch (i)
        {
        case 0:
            return s.Gamepad.sThumbLX;
        case 1:
            return s.Gamepad.sThumbLY;
        case 2:
            return s.Gamepad.sThumbRX;
        case 3:
            return s.Gamepad.sThumbRY;
        case 4:
            return (int16_t)(s.Gamepad.bLeftTrigger * 128);
        case 5:
            return (int16_t)(s.Gamepad.bRightTrigger * 128);
        default:
            return 0;
        }
    };
    for (int i = 0; i < 6; i++)
    {
        int16_t curr = getAxis(state, i);
        int16_t prev = getAxis(prevState, i);
        if (std::abs(curr) > threshold && std::abs(prev) <= threshold)
            return i;
    }
    return -1;
#else
    for (int i = 0; i < 6; i++)
    {
        bool active = std::abs(currAxes[i]) > threshold;
        bool wasActive = std::abs(prevAxes[i]) > threshold;
        if (active && !wasActive)
            return i;
    }
    return -1;
#endif
}

const char *Controller::buttonName(int index)
{
#ifdef _WIN32
    static const char *names[] = {
        "DPad Up", "DPad Down", "DPad Left", "DPad Right",
        "Start", "Back", "L3", "R3",
        "LB", "RB", nullptr, nullptr,
        "A", "B", "X", "Y"};
    if (index >= 0 && index < 16 && names[index])
        return names[index];
    return "Unknown";
#else
    static const char *names[] = {
        "A", "B", "X", "Y",
        "Back", "Guide", "Start",
        "L3", "R3", "LB", "RB",
        "DPad Up", "DPad Down", "DPad Left", "DPad Right",
        "Misc1", "R Paddle1", "L Paddle1", "R Paddle2", "L Paddle2",
        "Touchpad"};
    if (index >= 0 && index < 21)
        return names[index];
    return "Unknown";
#endif
}

const char *Controller::axisName(int index)
{
    static const char *names[] = {
        "Left Stick X", "Left Stick Y",
        "Right Stick X", "Right Stick Y",
        "Left Trigger", "Right Trigger"};
    if (index >= 0 && index < 6)
        return names[index];
    return "Unknown";
}
