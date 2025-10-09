#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "display.h"

Display::Display()
    : window(sf::VideoMode({800u, 600u}), "My SFML Window"),
      font(),
      text(font, "Initializing...", 24u) {}

Display::~Display()
{
    window.close();
}

void Display::draw()
{
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
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }

        // Rest of the main loop
    }
}
