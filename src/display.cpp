#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"
#include <iostream>
#include <cstdlib>

sf::Color parseHexColor(const std::string &hex)
{
    if (hex.size() != 6)
        return sf::Color::Black;
    char *end = nullptr;
    unsigned long val = std::strtoul(hex.c_str(), &end, 16);
    if (end != hex.c_str() + 6)
        return sf::Color::Black;
    return sf::Color(
        static_cast<uint8_t>((val >> 16) & 0xFF),
        static_cast<uint8_t>((val >> 8) & 0xFF),
        static_cast<uint8_t>(val & 0xFF));
}

Display::Display()
    : window(sf::VideoMode({800u, 600u}), "Input Viewer  |  Alt: Menu"),
      font(),
      text(font, "", 18u)
{
    if (!font.openFromFile("resources/OpenDyslexic-Regular.otf"))
        std::cerr << "Failed to load font" << std::endl;

    loadTextures();
}

Display::~Display()
{
    window.close();
}

bool Display::isOpen() const
{
    return window.isOpen();
}

ViewMode Display::getViewMode() const
{
    return viewMode;
}

float Display::navBarHeight() const
{
    return navBarVisible ? NAV_HEIGHT : 0.f;
}

void Display::loadTextures()
{
    auto load = [this](const std::string &key, const std::string &path) {
        sf::Texture tex;
        if (tex.loadFromFile(path))
            textures.emplace(key, std::move(tex));
    };

    std::vector<std::string> buttons = {"a", "b", "x", "y", "lb", "rb", "start", "select"};
    for (const auto &btn : buttons)
    {
        load(btn + "-outline", "resources/buttons/outline/" + btn + "-outline.png");
        load(btn + "-filled", "resources/buttons/filled/" + btn + "-filled.png");
        load(btn + "-pressed", "resources/buttons/pressed/" + btn + "-pressed.png");
    }

    load("lt-outline", "resources/buttons/outline/lt-outline.png");
    load("lt-filled", "resources/buttons/filled/lt-filled.png");
    load("lt-pressed", "resources/buttons/pressed/lt-pressed.png");
    load("rt-outline", "resources/buttons/outline/rt-outline.png");
    load("rt-filled", "resources/buttons/filled/rt-filled.png");
    load("rt-pressed", "resources/buttons/pressed/rt-pressed.png");

    load("dpad-gate", "resources/buttons/joysticks/d-pad-gate.png");
    load("dpad-gate-filled", "resources/buttons/filled/d-pad-gate-filled.png");
    load("dpad-pressed-up", "resources/buttons/pressed/d-pad-pressed-up.png");
    load("dpad-pressed-down", "resources/buttons/pressed/d-pad-pressed-down.png");
    load("dpad-pressed-left", "resources/buttons/pressed/d-pad-pressed-left.png");
    load("dpad-pressed-right", "resources/buttons/pressed/d-pad-pressed-right.png");

    load("lstick-gate", "resources/buttons/joysticks/joystick-gate.png");
    load("lstick-gate-filled", "resources/buttons/filled/joystick-gate-filled.png");
    load("lstick", "resources/buttons/joysticks/joystick.png");
    load("lstick-filled", "resources/buttons/filled/joystick-filled.png");
    load("lstick-ribs", "resources/buttons/joysticks/joystick-ribs.png");
    load("lstick-ribs-filled", "resources/buttons/filled/joystick-ribs-filled.png");
    load("rstick-gate", "resources/buttons/joysticks/c-stick-gate.png");
    load("rstick-gate-filled", "resources/buttons/filled/c-stick-gate-filled.png");
    load("rstick", "resources/buttons/joysticks/c-stick.png");
    load("rstick-filled", "resources/buttons/filled/c-stick-filled.png");
    load("rstick-ribs", "resources/buttons/joysticks/c-stick-ribs.png");
    load("rstick-ribs-filled", "resources/buttons/filled/c-stick-ribs-filled.png");
}

void Display::drawSpriteCentered(const std::string &key, float cx, float cy, float scale)
{
    auto it = textures.find(key);
    if (it == textures.end())
        return;

    sf::Sprite sprite(it->second);
    auto size = it->second.getSize();
    sprite.setOrigin({size.x / 2.f, size.y / 2.f});
    sprite.setScale({scale, scale});
    sprite.setPosition({cx, cy});
    window.draw(sprite);
}

void Display::drawSpritePartialFill(const std::string &key, float cx, float cy, float scale, float fill, bool fromLeft)
{
    auto it = textures.find(key);
    if (it == textures.end() || fill <= 0.f)
        return;

    if (fill > 1.f)
        fill = 1.f;

    auto size = it->second.getSize();
    int w = static_cast<int>(size.x);
    int h = static_cast<int>(size.y);
    int visibleW = static_cast<int>(w * fill);

    sf::IntRect rect;
    if (fromLeft)
        rect = sf::IntRect({0, 0}, {visibleW, h});
    else
        rect = sf::IntRect({w - visibleW, 0}, {visibleW, h});

    sf::Sprite sprite(it->second, rect);
    float originX = fromLeft
        ? size.x / 2.f
        : size.x / 2.f - (w - visibleW);
    sprite.setOrigin({originX, size.y / 2.f});
    sprite.setScale({scale, scale});
    sprite.setPosition({cx, cy});
    window.draw(sprite);
}

void Display::drawNavBar()
{
    if (!navBarVisible)
        return;

    sf::RectangleShape bar(sf::Vector2f(800.f, NAV_HEIGHT));
    bar.setFillColor(sf::Color(40, 40, 50));
    bar.setOutlineColor(sf::Color(70, 70, 90));
    bar.setOutlineThickness(1.f);
    window.draw(bar);

    struct Tab
    {
        const char *label;
        ViewMode mode;
    };
    Tab tabs[] = {{"Main", ViewMode::Main}, {"Layout", ViewMode::Layout}, {"Settings", ViewMode::Settings}};

    float tabW = 800.f / 3.f;
    for (int i = 0; i < 3; i++)
    {
        float x = i * tabW;
        bool active = (viewMode == tabs[i].mode);

        if (active)
        {
            sf::RectangleShape highlight(sf::Vector2f(tabW, NAV_HEIGHT));
            highlight.setPosition({x, 0.f});
            highlight.setFillColor(sf::Color(60, 60, 90));
            window.draw(highlight);
        }

        text.setCharacterSize(14u);
        text.setFillColor(active ? sf::Color::White : sf::Color(140, 140, 140));
        text.setString(tabs[i].label);
        auto bounds = text.getLocalBounds();
        text.setPosition({x + (tabW - bounds.size.x) / 2.f, (NAV_HEIGHT - bounds.size.y) / 2.f - 2.f});
        window.draw(text);
    }
}
