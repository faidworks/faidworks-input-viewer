#include <SFML/Graphics.hpp>
#include "display.h"
#include <iostream>
#include <algorithm>
#include <cmath>

static const float ROW_HEIGHT = 32.f;
static const float ROW_START_Y = 100.f;
static const float ROW_X = 50.f;
static const float ROW_WIDTH = 700.f;
static const float NAV_HEIGHT = 30.f;
static const float BTN_SCALE = 0.5f;
static const float WIDGET_SCALE = 0.85f;

static std::string toLower(const std::string &s)
{
    std::string r = s;
    for (char &c : r)
        c = std::tolower(c);
    return r;
}

Display::Display()
    : window(sf::VideoMode({800u, 600u}), "Input Viewer"),
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

    load("lt-outline", "resources/buttons/outline/analog-outline.png");
    load("lt-filled", "resources/buttons/filled/analog-filled.png");
    load("rt-outline", "resources/buttons/outline/analog-outline.png");
    load("rt-filled", "resources/buttons/filled/analog-filled.png");

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

static bool isMappingActive(const InputMapping &mapping, const Controller &controller)
{
    switch (mapping.type)
    {
    case InputType::Key:
        return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Scancode>(mapping.code));
    case InputType::GamepadButton:
        return controller.isButtonPressed(mapping.code);
    case InputType::GamepadAxis:
        return std::abs(controller.getAxisValue(mapping.code)) > 16000;
    default:
        return false;
    }
}

void Display::processEvents(Settings &settings, Controller &controller)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }

        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                window.close();
                return;
            }

            if ((keyPressed->scancode == sf::Keyboard::Scancode::LAlt ||
                 keyPressed->scancode == sf::Keyboard::Scancode::RAlt) &&
                detectingIndex < 0)
            {
                navBarVisible = !navBarVisible;
                continue;
            }

            if (detectingIndex >= 0 && viewMode == ViewMode::Settings)
            {
                const std::string &btn = GAMEPAD_BUTTONS[detectingIndex];
                InputMapping mapping;
                mapping.type = InputType::Key;
                mapping.code = static_cast<int>(keyPressed->scancode);
                mapping.name = sf::Keyboard::getDescription(keyPressed->scancode).toAnsiString();
                settings.mappings[btn] = mapping;
                detectingIndex = -1;
                continue;
            }
        }

        if (const auto *scroll = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            if (viewMode == ViewMode::Settings)
            {
                settingsScroll -= scroll->delta * 30.f;
                float maxScroll = std::max(0.f, ROW_START_Y + (float)(GAMEPAD_BUTTONS.size() + 1) * ROW_HEIGHT - 600.f + navBarHeight());
                if (settingsScroll < 0.f)
                    settingsScroll = 0.f;
                if (settingsScroll > maxScroll)
                    settingsScroll = maxScroll;
            }
        }

        if (const auto *mouseBtn = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseBtn->button == sf::Mouse::Button::Left)
            {
                float mx = static_cast<float>(mouseBtn->position.x);
                float my = static_cast<float>(mouseBtn->position.y);

                if (navBarVisible && my < NAV_HEIGHT)
                {
                    float tabW = 800.f / 3.f;
                    int tabIdx = static_cast<int>(mx / tabW);
                    ViewMode modes[] = {ViewMode::Main, ViewMode::Layout, ViewMode::Settings};
                    if (tabIdx >= 0 && tabIdx < 3)
                        viewMode = modes[tabIdx];
                    continue;
                }

                if (viewMode == ViewMode::Settings)
                {
                    float offy = navBarHeight();

                    float toggleY = ROW_START_Y + offy - settingsScroll;
                    if (mx >= ROW_X && mx <= ROW_X + ROW_WIDTH && my >= toggleY && my <= toggleY + ROW_HEIGHT - 2.f)
                    {
                        settings.activeStyle = (settings.activeStyle == ActiveStyle::Filled)
                                                   ? ActiveStyle::Pressed
                                                   : ActiveStyle::Filled;
                        continue;
                    }

                    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
                    {
                        float rowY = ROW_START_Y + (i + 1) * ROW_HEIGHT + offy - settingsScroll;
                        if (mx >= ROW_X && mx <= ROW_X + ROW_WIDTH && my >= rowY && my <= rowY + ROW_HEIGHT - 2.f)
                        {
                            detectingIndex = i;
                            break;
                        }
                    }
                }

                if (viewMode == ViewMode::Layout)
                {
                    float offy = navBarHeight();
                    float hitSize = 50.f;

                    for (const auto &elem : LAYOUT_ELEMENTS)
                    {
                        auto it = settings.layout.find(elem);
                        if (it == settings.layout.end())
                            continue;

                        float ex = it->second.x;
                        float ey = it->second.y + offy;

                        if (mx >= ex - hitSize && mx <= ex + hitSize &&
                            my >= ey - hitSize && my <= ey + hitSize)
                        {
                            draggingElement = elem;
                            dragOffset = {mx - ex, my - ey};
                            break;
                        }
                    }
                }
            }
        }

        if (const auto *mouseBtn = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseBtn->button == sf::Mouse::Button::Left)
                draggingElement.clear();
        }

        if (const auto *mouseMove = event->getIf<sf::Event::MouseMoved>())
        {
            if (!draggingElement.empty() && viewMode == ViewMode::Layout)
            {
                float mx = static_cast<float>(mouseMove->position.x);
                float my = static_cast<float>(mouseMove->position.y);
                float offy = navBarHeight();

                settings.layout[draggingElement] = {
                    mx - dragOffset.x,
                    my - dragOffset.y - offy};
            }
        }
    }

    if (detectingIndex >= 0 && viewMode == ViewMode::Settings)
    {
        int btn = controller.getNewlyPressedButton();
        if (btn >= 0)
        {
            const std::string &target = GAMEPAD_BUTTONS[detectingIndex];
            InputMapping mapping;
            mapping.type = InputType::GamepadButton;
            mapping.code = btn;
            mapping.name = Controller::buttonName(btn);
            settings.mappings[target] = mapping;
            detectingIndex = -1;
            return;
        }

        int axis = controller.getNewlyActiveAxis();
        if (axis >= 0)
        {
            const std::string &target = GAMEPAD_BUTTONS[detectingIndex];
            InputMapping mapping;
            mapping.type = InputType::GamepadAxis;
            mapping.code = axis;
            mapping.name = Controller::axisName(axis);
            settings.mappings[target] = mapping;
            detectingIndex = -1;
            return;
        }
    }
}

static void drawButtonImage(Display &d, const std::map<std::string, sf::Texture> &textures,
                            sf::RenderWindow &window, sf::Text &text,
                            const std::string &btn, float cx, float cy,
                            bool active, const std::string &activeSuffix)
{
    std::string prefix = toLower(btn);

    std::string key = prefix + (active ? activeSuffix : "-outline");
    if (active && textures.find(key) == textures.end())
    {
        key = prefix + "-filled";
        if (textures.find(key) == textures.end())
            key = prefix + "-pressed";
    }

    if (textures.find(key) != textures.end())
    {
        auto it = textures.find(key);
        sf::Sprite sprite(it->second);
        auto size = it->second.getSize();
        sprite.setOrigin({size.x / 2.f, size.y / 2.f});
        sprite.setScale({BTN_SCALE, BTN_SCALE});
        sprite.setPosition({cx, cy - 8.f});
        window.draw(sprite);
    }
    else
    {
        text.setCharacterSize(20u);
        text.setFillColor(active ? sf::Color::White : sf::Color(180, 180, 180));
        text.setString(btn);
        auto bounds = text.getLocalBounds();
        text.setPosition({cx - bounds.size.x / 2.f, cy - bounds.size.y / 2.f - 8.f});
        window.draw(text);
    }

}

void Display::render(const Settings &settings, const Controller &controller)
{
    window.clear(sf::Color::Black);
    drawNavBar();
    float offy = navBarHeight();

    if (!navBarVisible)
    {
        text.setCharacterSize(14u);
        text.setFillColor(sf::Color(150, 150, 150));
        text.setString("Alt: Menu");
        text.setPosition({10.f, 5.f});
        window.draw(text);
    }

    auto isActive = [&](const std::string &btn) -> bool {
        auto it = settings.mappings.find(btn);
        if (it == settings.mappings.end() || it->second.type == InputType::None)
            return false;
        return isMappingActive(it->second, controller);
    };

    std::string activeSuffix = (settings.activeStyle == ActiveStyle::Pressed) ? "-pressed" : "-filled";

    static const std::vector<std::string> gridButtons = {
        "A", "B", "X", "Y", "Start",
        "LB", "RB", "LT", "RT", "Select"};

    for (const auto &btn : gridButtons)
    {
        auto it = settings.layout.find(btn);
        if (it == settings.layout.end())
            continue;
        float cx = it->second.x;
        float cy = it->second.y + offy;
        drawButtonImage(*this, textures, window, text, btn, cx, cy, isActive(btn), activeSuffix);
    }

    // D-pad
    {
        auto it = settings.layout.find("DPad");
        if (it != settings.layout.end())
        {
            float cx = it->second.x;
            float cy = it->second.y + offy;

            bool dpadFilled = (settings.activeStyle == ActiveStyle::Filled);
            drawSpriteCentered(dpadFilled ? "dpad-gate-filled" : "dpad-gate", cx, cy, WIDGET_SCALE);
            if (isActive("DPad Up"))
                drawSpriteCentered("dpad-pressed-up", cx, cy, WIDGET_SCALE);
            if (isActive("DPad Down"))
                drawSpriteCentered("dpad-pressed-down", cx, cy, WIDGET_SCALE);
            if (isActive("DPad Left"))
                drawSpriteCentered("dpad-pressed-left", cx, cy, WIDGET_SCALE);
            if (isActive("DPad Right"))
                drawSpriteCentered("dpad-pressed-right", cx, cy, WIDGET_SCALE);
        }
    }

    // Joysticks
    bool filled = (settings.activeStyle == ActiveStyle::Filled);

    auto drawStick = [&](const std::string &layoutKey,
                         const std::string &baseKey,
                         const std::string &axisXBtn, const std::string &axisYBtn,
                         const std::string &l3Btn) {
        auto it = settings.layout.find(layoutKey);
        if (it == settings.layout.end())
            return;

        float cx = it->second.x;
        float cy = it->second.y + offy;

        std::string gateKey = baseKey + "-gate" + (filled ? "-filled" : "");
        drawSpriteCentered(gateKey, cx, cy, WIDGET_SCALE);

        float dx = 0.f, dy = 0.f;
        auto itX = settings.mappings.find(axisXBtn);
        if (itX != settings.mappings.end() && itX->second.type == InputType::GamepadAxis)
            dx = controller.getAxisValue(itX->second.code) / 32768.f;
        auto itY = settings.mappings.find(axisYBtn);
        if (itY != settings.mappings.end() && itY->second.type == InputType::GamepadAxis)
            dy = controller.getAxisValue(itY->second.code) / 32768.f;

        float maxDisp = 20.f * WIDGET_SCALE;
        float stickX = cx + dx * maxDisp;
        float stickY = cy + dy * maxDisp;

        bool l3Active = isActive(l3Btn);
        std::string sKey;
        if (l3Active)
            sKey = baseKey + "-ribs" + (filled ? "-filled" : "");
        else
            sKey = baseKey + (filled ? "-filled" : "");

        if (!textures.count(sKey))
            sKey = baseKey;
        drawSpriteCentered(sKey, stickX, stickY, WIDGET_SCALE);
    };

    drawStick("LStick", "lstick", "LStick X", "LStick Y", "L3");
    drawStick("RStick", "rstick", "RStick X", "RStick Y", "R3");

    window.display();
}

void Display::renderLayout(Settings &settings)
{
    window.clear(sf::Color(20, 20, 30));
    drawNavBar();
    float offy = navBarHeight();

    if (!navBarVisible)
    {
        text.setCharacterSize(14u);
        text.setFillColor(sf::Color(150, 150, 150));
        text.setString("Alt: Menu");
        text.setPosition({10.f, 5.f});
        window.draw(text);
    }

    text.setCharacterSize(12u);
    text.setFillColor(sf::Color(80, 80, 100));
    text.setString("Drag elements to reposition");
    text.setPosition({10.f, 580.f});
    window.draw(text);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    for (const auto &elem : LAYOUT_ELEMENTS)
    {
        auto it = settings.layout.find(elem);
        if (it == settings.layout.end())
            continue;

        float cx = it->second.x;
        float cy = it->second.y + offy;
        float hitSize = 50.f;

        bool hovered = (mx >= cx - hitSize && mx <= cx + hitSize &&
                        my >= cy - hitSize && my <= cy + hitSize);
        bool dragging = (draggingElement == elem);

        if (elem == "DPad")
        {
            drawSpriteCentered("dpad-gate", cx, cy, WIDGET_SCALE);
        }
        else if (elem == "LStick")
        {
            drawSpriteCentered("lstick-gate", cx, cy, WIDGET_SCALE);
            drawSpriteCentered("lstick", cx, cy, WIDGET_SCALE);
        }
        else if (elem == "RStick")
        {
            drawSpriteCentered("rstick-gate", cx, cy, WIDGET_SCALE);
            drawSpriteCentered("rstick", cx, cy, WIDGET_SCALE);
        }
        else
        {
            std::string prefix = toLower(elem);
            std::string key = prefix + "-outline";
            if (textures.find(key) != textures.end())
            {
                auto tit = textures.find(key);
                sf::Sprite sprite(tit->second);
                auto size = tit->second.getSize();
                sprite.setOrigin({size.x / 2.f, size.y / 2.f});
                sprite.setScale({BTN_SCALE, BTN_SCALE});
                sprite.setPosition({cx, cy - 8.f});
                window.draw(sprite);
            }
        }

        if (hovered || dragging)
        {
            sf::RectangleShape box(sf::Vector2f(hitSize * 2.f, hitSize * 2.f));
            box.setPosition({cx - hitSize, cy - hitSize});
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(dragging ? sf::Color(100, 200, 255) : sf::Color(80, 80, 120));
            box.setOutlineThickness(1.f);
            window.draw(box);
        }

        text.setCharacterSize(11u);
        text.setFillColor(hovered || dragging ? sf::Color(180, 180, 255) : sf::Color(100, 100, 130));
        text.setString(elem);
        auto b = text.getLocalBounds();
        text.setPosition({cx - b.size.x / 2.f, cy + 42.f});
        window.draw(text);
    }

    window.display();
}

void Display::renderSettings(const Settings &settings)
{
    window.clear(sf::Color(30, 30, 40));
    drawNavBar();
    float offy = navBarHeight();

    if (!navBarVisible)
    {
        text.setCharacterSize(14u);
        text.setFillColor(sf::Color(150, 150, 150));
        text.setString("Alt: Menu");
        text.setPosition({10.f, 5.f});
        window.draw(text);
    }

    text.setCharacterSize(22u);
    text.setFillColor(sf::Color::White);
    text.setString("Input Mappings");
    text.setPosition({ROW_X, 30.f + offy});
    window.draw(text);

    float baseY = ROW_START_Y + offy;

    // Active style toggle
    {
        float rowY = baseY - settingsScroll;
        sf::RectangleShape row(sf::Vector2f(ROW_WIDTH, ROW_HEIGHT - 2.f));
        row.setPosition(sf::Vector2f(ROW_X, rowY));
        row.setFillColor(sf::Color(60, 50, 80));
        row.setOutlineColor(sf::Color(100, 80, 140));
        row.setOutlineThickness(1.f);
        window.draw(row);

        text.setCharacterSize(16u);
        text.setFillColor(sf::Color(200, 180, 255));
        text.setString("Active Style");
        text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
        window.draw(text);

        std::string val = (settings.activeStyle == ActiveStyle::Pressed) ? "Pressed" : "Filled";
        text.setString(val + "  (click to toggle)");
        text.setFillColor(sf::Color(150, 255, 150));
        text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
        window.draw(text);
    }

    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
    {
        float rowY = baseY + (i + 1) * ROW_HEIGHT - settingsScroll;
        if (rowY + ROW_HEIGHT < baseY || rowY > 600.f)
            continue;
        bool isDetecting = (detectingIndex == i);

        sf::RectangleShape row(sf::Vector2f(ROW_WIDTH, ROW_HEIGHT - 2.f));
        row.setPosition(sf::Vector2f(ROW_X, rowY));

        if (isDetecting)
            row.setFillColor(sf::Color(80, 80, 140));
        else
            row.setFillColor(sf::Color(50, 50, 60));

        row.setOutlineColor(sf::Color(80, 80, 100));
        row.setOutlineThickness(1.f);
        window.draw(row);

        text.setCharacterSize(16u);
        text.setFillColor(sf::Color::White);
        text.setString(GAMEPAD_BUTTONS[i]);
        text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
        window.draw(text);

        std::string mappingStr;
        if (isDetecting)
        {
            mappingStr = ">> Press any input... <<";
            text.setFillColor(sf::Color::Yellow);
        }
        else
        {
            auto it = settings.mappings.find(GAMEPAD_BUTTONS[i]);
            if (it != settings.mappings.end() && it->second.type != InputType::None)
            {
                std::string prefix;
                switch (it->second.type)
                {
                case InputType::Key:
                    prefix = "[Key] ";
                    break;
                case InputType::GamepadButton:
                    prefix = "[Btn] ";
                    break;
                case InputType::GamepadAxis:
                    prefix = "[Axis] ";
                    break;
                default:
                    break;
                }
                mappingStr = prefix + it->second.name;
                text.setFillColor(sf::Color(100, 255, 100));
            }
            else
            {
                mappingStr = "Not mapped";
                text.setFillColor(sf::Color(120, 120, 120));
            }
        }

        text.setString(mappingStr);
        text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
        window.draw(text);
    }

    window.display();
}
