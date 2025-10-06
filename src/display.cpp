#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "display.h"

Display::Display() 
    : window(sf::VideoMode(800u, 600u), "My SFML Window") // Ensure width and height are unsigned
{
    if (!font.loadFromFile("path/to/font.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    
    text.setFont(font);
    text.setString("Hello, SFML!");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
}

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
