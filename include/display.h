#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include "controller.h"

class Display
{
public:
    Display();
    ~Display();
    void render(const Controller::GamepadState &state);
    bool isOpen() const; // New method to check if the window is open

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;
};

#endif // DISPLAY_H
