#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "controller.h"
#include "settings.h"

enum class ViewMode { Main, Layout, Settings };

class Display
{
public:
    Display();
    ~Display();
    void processEvents(Settings &settings, Controller &controller);
    void render(const Settings &settings, const Controller &controller);
    void renderLayout(Settings &settings);
    void renderSettings(const Settings &settings);
    bool isOpen() const;
    ViewMode getViewMode() const;

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;
    ViewMode viewMode = ViewMode::Main;
    bool navBarVisible = false;
    int detectingIndex = -1;
    float settingsScroll = 0.f;

    std::map<std::string, sf::Texture> textures;
    sf::Shader tintShader;
    bool shaderLoaded = false;
    void loadTextures();
    void drawSpriteCentered(const std::string &key, float cx, float cy, float scale);
    void drawSpriteTinted(const std::string &key, float cx, float cy, float scale, sf::Color tint);
    void drawSpritePartialFill(const std::string &key, float cx, float cy, float scale, float fill, bool fromLeft, sf::Color tint = sf::Color::White);
    void drawNavBar();
    float navBarHeight() const;

    std::string draggingElement;
    sf::Vector2f dragOffset;

    bool editingBgColor = false;
    std::string bgColorInput;

    int editingColorIndex = -1;
    bool editingColorIsActive = false;
    std::string colorInput;
};

sf::Color parseHexColor(const std::string &hex);

#endif // DISPLAY_H
