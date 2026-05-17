#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>

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
    : window(sf::VideoMode({800u, 600u}), "Faidworks Input Viewer  |  Alt: Menu"),
      font(),
      text(font, "", 18u)
{
    if (!font.openFromFile("resources/fonts/OpenDyslexic-Regular.otf"))
        std::cerr << "Failed to load font" << std::endl;

    scanSystemFonts();
    loadTextures();

    if (tintShader.loadFromFile("resources/shaders/color_tint.frag", sf::Shader::Type::Fragment))
        shaderLoaded = true;
    else
        std::cerr << "Failed to load tint shader" << std::endl;
}

void Display::scanSystemFonts()
{
    namespace fs = std::filesystem;
    availableFonts.clear();
    availableFonts.push_back({"OpenDyslexic", "resources/fonts/OpenDyslexic-Regular.otf"});

    std::vector<std::string> fontDirs;
#ifdef _WIN32
    const char *windir = std::getenv("WINDIR");
    if (windir)
        fontDirs.push_back(std::string(windir) + "\\Fonts");
#elif __APPLE__
    fontDirs = {"/Library/Fonts", "/System/Library/Fonts"};
    const char *home = std::getenv("HOME");
    if (home)
        fontDirs.push_back(std::string(home) + "/Library/Fonts");
#else
    fontDirs = {"/usr/share/fonts", "/usr/local/share/fonts"};
    const char *home = std::getenv("HOME");
    if (home)
    {
        fontDirs.push_back(std::string(home) + "/.local/share/fonts");
        fontDirs.push_back(std::string(home) + "/.fonts");
    }
#endif

    for (const auto &dir : fontDirs)
    {
        try
        {
            if (!fs::exists(dir))
                continue;
            for (const auto &entry : fs::recursive_directory_iterator(dir))
            {
                if (!entry.is_regular_file())
                    continue;
                auto ext = toLower(entry.path().extension().string());
                if (ext != ".ttf" && ext != ".otf")
                    continue;
                sf::Font testFont;
                if (!testFont.openFromFile(entry.path().string()))
                    continue;
                if (testFont.getGlyph('A', 16, false).bounds.size == sf::Vector2f{0.f, 0.f})
                    continue;
                availableFonts.push_back({entry.path().stem().string(), entry.path().string()});
            }
        }
        catch (...)
        {
        }
    }

    std::sort(availableFonts.begin() + 1, availableFonts.end());
}

void Display::loadFont(const std::string &path)
{
    if (!font.openFromFile(path))
        std::cerr << "Failed to load font: " << path << std::endl;

    for (int i = 0; i < (int)availableFonts.size(); i++)
    {
        if (availableFonts[i].second == path)
        {
            fontIndex = i;
            return;
        }
    }
    fontIndex = 0;
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
    drawSpriteTinted(key, cx, cy, scale, sf::Color::White);
}

void Display::drawSpriteTinted(const std::string &key, float cx, float cy, float scale, sf::Color tint)
{
    auto it = textures.find(key);
    if (it == textures.end())
        return;

    sf::Sprite sprite(it->second);
    auto size = it->second.getSize();
    sprite.setOrigin({size.x / 2.f, size.y / 2.f});
    sprite.setScale({scale, scale});
    sprite.setPosition({cx, cy});

    if (shaderLoaded && tint != sf::Color::White)
    {
        sf::Glsl::Vec4 tintVec(tint.r / 255.f, tint.g / 255.f, tint.b / 255.f, tint.a / 255.f);
        tintShader.setUniform("tintColor", tintVec);
        tintShader.setUniform("texture", sf::Shader::CurrentTexture);
        window.draw(sprite, sf::RenderStates(&tintShader));
    }
    else
    {
        window.draw(sprite);
    }
}

void Display::drawSpritePartialFill(const std::string &key, float cx, float cy, float scale, float fill, bool fromLeft, sf::Color tint)
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

    if (shaderLoaded && tint != sf::Color::White)
    {
        sf::Glsl::Vec4 tintVec(tint.r / 255.f, tint.g / 255.f, tint.b / 255.f, tint.a / 255.f);
        tintShader.setUniform("tintColor", tintVec);
        tintShader.setUniform("texture", sf::Shader::CurrentTexture);
        window.draw(sprite, sf::RenderStates(&tintShader));
    }
    else
    {
        window.draw(sprite);
    }
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
