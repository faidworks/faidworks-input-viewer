#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "display.h"

Display::Display() 
    : window(sf::VideoMode({800u, 600u}), "My SFML Window"),
        font("../resources/openDyslexic.otf"),
        text(font, "Initializing...", 24) {}

Display::~Display()
{
    window.close();
}

void Display::draw() {
    window.clear();
    window.draw(text);
    window.display();
}

bool Display::isOpen() const
{
    return window.isOpen();
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
