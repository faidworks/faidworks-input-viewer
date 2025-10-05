#include "display.h"
#include <iostream>

Display::Display()
{
    // Create the window
    window.create(sf::VideoMode(800, 600), "Controller Input Display");

    // Load a font
    if (!font.loadFromFile("resources/OpenDyslexic-Regular.otf"))
    {
        std::cerr << "Failed to load font\n";
    }

    // Set up the text
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
}

bool Display::isOpen() const
{
    return window.isOpen();
}

Display::~Display()
{
    window.close();
}

void Display::render(const Controller::GamepadState &state)
{
    // Clear the window
    window.clear(sf::Color::Black);

    // Prepare text to display controller state
    std::string inputText = "Buttons: " + std::to_string(state.buttons) +
                            "\nLeft Thumb: (" + std::to_string(state.leftThumbX) + ", " +
                            std::to_string(state.leftThumbY) + ")" +
                            "\nRight Thumb: (" + std::to_string(state.rightThumbX) + ", " +
                            std::to_string(state.rightThumbY) + ")";

    text.setString(inputText);

    // Draw the text
    window.draw(text);
    window.display();

    // Handle events
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
    }
}
