#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include "controller.h"
#include "settings.h"

enum class ViewMode { Main, Settings };

class Display
{
public:
    Display();
    ~Display();
    void processEvents(Settings &settings, Controller &controller);
    void render(const Settings &settings, const Controller &controller);
    void renderSettings(const Settings &settings);
    bool isOpen() const;
    ViewMode getViewMode() const;

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;
    ViewMode viewMode = ViewMode::Settings;
    int detectingIndex = -1;
    float settingsScroll = 0.f;
};

#endif // DISPLAY_H
