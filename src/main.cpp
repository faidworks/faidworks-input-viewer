#include <iostream>
#include "controller.h"
#include "settings.h"
#include "display.h"

int main()
{
    Settings settings;
    Display display;
    Controller controller;

    // Load settings
    settings.load();

    // Main loop
    while (true)
    {
        controller.update();

        // Get the current gamepad state
        auto state = controller.getState();

        // Render the current state
        display.render(state);

        // Handle user input for quitting
        if (!display.isOpen())
        { // If the window is closed, break the loop
            break;
        }
    }

    settings.save();
    return 0;
}
