#include "controller.h"
#include <iostream>

Controller::Controller()
{
#ifdef _WIN32
    ZeroMemory(&state, sizeof(XINPUT_STATE));
#else
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        controller = SDL_GameControllerOpen(0);
        joystick = SDL_JoystickOpen(0);
        if (controller == nullptr)
        {
            std::cerr << "Failed to open game controller! SDL Error: " << SDL_GetError() << std::endl;
        }
    }
#endif
}

Controller::~Controller()
{
#ifdef _WIN32
    // No specific clean-up required for XInput
#else
    if (controller)
    {
        SDL_GameControllerClose(controller);
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
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A) == SDL_PRESSED)
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
    // Translating SDL buttons to a custom state definition
    gamepadState.leftThumbX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    gamepadState.leftThumbY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    gamepadState.rightThumbX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
    gamepadState.rightThumbY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
#endif
    return gamepadState;
}
