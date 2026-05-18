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
    Settings defaultSettings;
    loadTextures(defaultSettings);

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

void Display::toggleHistoryWindow()
{
    if (historyOpen)
    {
        historyWindow.close();
        historyOpen = false;
    }
    else
    {
        historyWindow.create(sf::VideoMode({400u, 500u}), "Input History");
        historyOpen = true;
    }
}

void Display::setFramerateLimit(int limit)
{
    window.setFramerateLimit(limit > 0 ? static_cast<unsigned int>(limit) : 0);
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

struct TextureEntry {
    const char *key;
    const char *inactiveDefault;
    const char *activeDefaultFilled;
    const char *activeDefaultPressed;
    const char *filename;
    bool isActive;
};

static const TextureEntry TEXTURE_TABLE[] = {
    // Standard buttons
    {"a-inactive",            "resources/buttons/filled/inactive/a.png",      nullptr, nullptr, "a.png", false},
    {"a-active",              nullptr, "resources/buttons/filled/active/a.png",      "resources/buttons/pressed/active/a.png", "a.png", true},
    {"b-inactive",            "resources/buttons/filled/inactive/b.png",      nullptr, nullptr, "b.png", false},
    {"b-active",              nullptr, "resources/buttons/filled/active/b.png",      "resources/buttons/pressed/active/b.png", "b.png", true},
    {"x-inactive",            "resources/buttons/filled/inactive/x.png",      nullptr, nullptr, "x.png", false},
    {"x-active",              nullptr, "resources/buttons/filled/active/x.png",      "resources/buttons/pressed/active/x.png", "x.png", true},
    {"y-inactive",            "resources/buttons/filled/inactive/y.png",      nullptr, nullptr, "y.png", false},
    {"y-active",              nullptr, "resources/buttons/filled/active/y.png",      "resources/buttons/pressed/active/y.png", "y.png", true},
    {"lb-inactive",           "resources/buttons/filled/inactive/lb.png",     nullptr, nullptr, "lb.png", false},
    {"lb-active",             nullptr, "resources/buttons/filled/active/lb.png",     "resources/buttons/pressed/active/lb.png", "lb.png", true},
    {"rb-inactive",           "resources/buttons/filled/inactive/rb.png",     nullptr, nullptr, "rb.png", false},
    {"rb-active",             nullptr, "resources/buttons/filled/active/rb.png",     "resources/buttons/pressed/active/rb.png", "rb.png", true},
    {"lt-inactive",           "resources/buttons/filled/inactive/lt.png",     nullptr, nullptr, "lt.png", false},
    {"lt-active",             nullptr, "resources/buttons/filled/active/lt.png",     "resources/buttons/pressed/active/lt.png", "lt.png", true},
    {"rt-inactive",           "resources/buttons/filled/inactive/rt.png",     nullptr, nullptr, "rt.png", false},
    {"rt-active",             nullptr, "resources/buttons/filled/active/rt.png",     "resources/buttons/pressed/active/rt.png", "rt.png", true},
    {"start-inactive",        "resources/buttons/filled/inactive/start.png",  nullptr, nullptr, "start.png", false},
    {"start-active",          nullptr, "resources/buttons/filled/active/start.png",  "resources/buttons/pressed/active/start.png", "start.png", true},
    {"select-inactive",       "resources/buttons/filled/inactive/select.png", nullptr, nullptr, "select.png", false},
    {"select-active",         nullptr, "resources/buttons/filled/active/select.png", "resources/buttons/pressed/active/select.png", "select.png", true},

    // D-pad
    {"dpad-gate-inactive",    "resources/buttons/filled/inactive/d-pad-gate.png",    nullptr, nullptr, "d-pad-gate.png", false},
    {"dpad-gate-active",      nullptr, "resources/buttons/filled/active/d-pad-gate.png",    "resources/buttons/pressed/active/d-pad-gate.png", "d-pad-gate.png", true},
    {"dpad-active-up",        nullptr, "resources/buttons/filled/active/d-pad-up.png",      "resources/buttons/pressed/active/d-pad-up.png", "d-pad-up.png", true},
    {"dpad-active-down",      nullptr, "resources/buttons/filled/active/d-pad-down.png",    "resources/buttons/pressed/active/d-pad-down.png", "d-pad-down.png", true},
    {"dpad-active-left",      nullptr, "resources/buttons/filled/active/d-pad-left.png",    "resources/buttons/pressed/active/d-pad-left.png", "d-pad-left.png", true},
    {"dpad-active-right",     nullptr, "resources/buttons/filled/active/d-pad-right.png",   "resources/buttons/pressed/active/d-pad-right.png", "d-pad-right.png", true},

    // Left stick
    {"lstick-gate-inactive",  "resources/buttons/filled/inactive/joystick-gate.png", nullptr, nullptr, "joystick-gate.png", false},
    {"lstick-gate-active",    nullptr, "resources/buttons/filled/active/joystick-gate.png",  "resources/buttons/pressed/active/joystick-gate.png", "joystick-gate.png", true},
    {"lstick-inactive",       "resources/buttons/filled/inactive/joystick.png",      nullptr, nullptr, "joystick.png", false},
    {"lstick-active",         nullptr, "resources/buttons/filled/active/joystick.png",       "resources/buttons/pressed/active/joystick.png", "joystick.png", true},
    {"lstick-ribs-inactive",  "resources/buttons/filled/inactive/joystick-ribs.png", nullptr, nullptr, "joystick-ribs.png", false},
    {"lstick-ribs-active",    nullptr, "resources/buttons/filled/active/joystick-ribs.png",  "resources/buttons/pressed/active/joystick-ribs.png", "joystick-ribs.png", true},

    // Right stick
    {"rstick-gate-inactive",  "resources/buttons/filled/inactive/c-stick-gate.png",  nullptr, nullptr, "c-stick-gate.png", false},
    {"rstick-gate-active",    nullptr, "resources/buttons/filled/active/c-stick-gate.png",   "resources/buttons/pressed/active/c-stick-gate.png", "c-stick-gate.png", true},
    {"rstick-inactive",       "resources/buttons/filled/inactive/c-stick.png",       nullptr, nullptr, "c-stick.png", false},
    {"rstick-active",         nullptr, "resources/buttons/filled/active/c-stick.png",        "resources/buttons/pressed/active/c-stick.png", "c-stick.png", true},
    {"rstick-ribs-inactive",  "resources/buttons/filled/inactive/c-stick-ribs.png",  nullptr, nullptr, "c-stick-ribs.png", false},
    {"rstick-ribs-active",    nullptr, "resources/buttons/filled/active/c-stick-ribs.png",   "resources/buttons/pressed/active/c-stick-ribs.png", "c-stick-ribs.png", true},

    // Misc
    {"disconnected",          "resources/buttons/misc/disconnected.png",             nullptr, nullptr, "disconnected.png", false},
};

void Display::loadTextures(const Settings &settings)
{
    namespace fs = std::filesystem;
    bool usePressed = (settings.presetConfig.builtinActive == "pressed");

    for (const auto &entry : TEXTURE_TABLE)
    {
        std::string path;

        auto ovIt = settings.presetConfig.imageOverrides.find(entry.key);
        if (ovIt != settings.presetConfig.imageOverrides.end() && !ovIt->second.empty())
            path = ovIt->second;
        else if (!settings.presetConfig.imageFolder.empty())
        {
            std::string subfolder = entry.isActive ? "active" : "inactive";
            std::string folderPath = settings.presetConfig.imageFolder + "/" + subfolder + "/" + entry.filename;
            if (fs::exists(folderPath))
                path = folderPath;
        }

        if (path.empty())
        {
            if (entry.isActive)
                path = usePressed ? entry.activeDefaultPressed : entry.activeDefaultFilled;
            else
                path = entry.inactiveDefault;
        }

        sf::Texture tex;
        if (tex.loadFromFile(path))
            textures.emplace(entry.key, std::move(tex));
    }
}

void Display::reloadTextures(const Settings &settings)
{
    textures.clear();
    loadTextures(settings);
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

    float tabW = 800.f / 4.f;

    struct Tab
    {
        const char *label;
        ViewMode mode;
        bool isToggle;
    };
    Tab tabs[] = {
        {"Main", ViewMode::Main, false},
        {"History", ViewMode::Main, true},
        {"Layout", ViewMode::Layout, false},
        {"Settings", ViewMode::Settings, false}
    };

    for (int i = 0; i < 4; i++)
    {
        float x = i * tabW;
        bool active = tabs[i].isToggle ? historyOpen : (viewMode == tabs[i].mode);

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
