#include "controller.h"
#include <iostream>

Controller::Controller()
{
#ifdef _WIN32
    ZeroMemory(&state, sizeof(XINPUT_STATE));
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
    XInputGetState(0, &state);
#else
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        /* Handle SDL events if necessary */
    }
    if (gamepad && SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH))
    {
        // Example: Handle button press
    }
#endif
}

Controller::GamepadState Controller::getState() const
{
    GamepadState gamepadState;
#ifdef _WIN32
    gamepadState.buttons = state.Gamepad.wButtons;
    gamepadState.leftThumbX = state.Gamepad.sThumbLX;
    gamepadState.leftThumbY = state.Gamepad.sThumbLY;
    gamepadState.rightThumbX = state.Gamepad.sThumbRX;
    gamepadState.rightThumbY = state.Gamepad.sThumbRY;
#else
    gamepadState.buttons = 0;
    gamepadState.leftThumbX = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    gamepadState.leftThumbY = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
    gamepadState.rightThumbX = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    gamepadState.rightThumbY = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
#endif
    return gamepadState;
}
