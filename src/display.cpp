#include <SFML/Graphics.hpp>
#include "display.h"
#include <iostream>
#include <algorithm>
#include <cmath>

static const float ROW_HEIGHT = 32.f;
static const float ROW_START_Y = 100.f;
static const float ROW_X = 50.f;
static const float ROW_WIDTH = 700.f;

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

    for (const std::string &btn : {"lt", "rt"})
    {
        load(btn + "-outline", "resources/buttons/outline/" + btn + "-outline.png");
        load(btn + "-filled", "resources/buttons/filled/" + btn + "-filled.png");
    }

    load("dpad-gate", "resources/buttons/joysticks/d-pad-gate.png");
    load("dpad-pressed-up", "resources/buttons/pressed/d-pad-pressed-up.png");
    load("dpad-pressed-down", "resources/buttons/pressed/d-pad-pressed-down.png");
    load("dpad-pressed-left", "resources/buttons/pressed/d-pad-pressed-left.png");
    load("dpad-pressed-right", "resources/buttons/pressed/d-pad-pressed-right.png");

    load("lstick-gate", "resources/buttons/joysticks/joystick-gate.png");
    load("lstick", "resources/buttons/joysticks/joystick.png");
    load("lstick-ribs", "resources/buttons/joysticks/joystick-ribs.png");
    load("rstick-gate", "resources/buttons/joysticks/c-stick-gate.png");
    load("rstick", "resources/buttons/joysticks/c-stick.png");
    load("rstick-ribs", "resources/buttons/joysticks/c-stick-solid.png");
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

            if (keyPressed->scancode == sf::Keyboard::Scancode::Tab && detectingIndex < 0)
            {
                viewMode = (viewMode == ViewMode::Main) ? ViewMode::Settings : ViewMode::Main;
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
                float maxScroll = std::max(0.f, ROW_START_Y + (float)(GAMEPAD_BUTTONS.size() + 1) * ROW_HEIGHT - 600.f);
                if (settingsScroll < 0.f)
                    settingsScroll = 0.f;
                if (settingsScroll > maxScroll)
                    settingsScroll = maxScroll;
            }
        }

        if (const auto *mouseBtn = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseBtn->button == sf::Mouse::Button::Left && viewMode == ViewMode::Settings)
            {
                float mx = static_cast<float>(mouseBtn->position.x);
                float my = static_cast<float>(mouseBtn->position.y);

                float toggleY = ROW_START_Y - settingsScroll;
                if (mx >= ROW_X && mx <= ROW_X + ROW_WIDTH && my >= toggleY && my <= toggleY + ROW_HEIGHT - 2.f)
                {
                    settings.activeStyle = (settings.activeStyle == ActiveStyle::Filled)
                                               ? ActiveStyle::Pressed
                                               : ActiveStyle::Filled;
                    continue;
                }

                for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
                {
                    float rowY = ROW_START_Y + (i + 1) * ROW_HEIGHT - settingsScroll;
                    if (mx >= ROW_X && mx <= ROW_X + ROW_WIDTH && my >= rowY && my <= rowY + ROW_HEIGHT - 2.f)
                    {
                        detectingIndex = i;
                        break;
                    }
                }
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

void Display::render(const Settings &settings, const Controller &controller)
{
    window.clear(sf::Color::Black);

    text.setCharacterSize(14u);
    text.setFillColor(sf::Color(150, 150, 150));
    text.setString("Tab: Settings");
    text.setPosition({10.f, 5.f});
    window.draw(text);

    auto isActive = [&](const std::string &btn) -> bool {
        auto it = settings.mappings.find(btn);
        if (it == settings.mappings.end() || it->second.type == InputType::None)
            return false;
        return isMappingActive(it->second, controller);
    };

    std::string activeSuffix = (settings.activeStyle == ActiveStyle::Pressed) ? "-pressed" : "-filled";

    // --- Regular buttons in grid ---
    static const std::vector<std::string> gridButtons = {
        "A", "B", "X", "Y", "Start",
        "LB", "RB", "LT", "RT", "Select"};

    static const int GRID_COLS = 5;
    static const float CELL_W = 110.f;
    static const float CELL_H = 100.f;
    static const float GRID_START_X = (800.f - GRID_COLS * CELL_W) / 2.f;
    static const float GRID_START_Y = 30.f;
    static const float BTN_SCALE = 0.5f;

    for (int i = 0; i < (int)gridButtons.size(); i++)
    {
        int col = i % GRID_COLS;
        int row = i / GRID_COLS;
        float cx = GRID_START_X + col * CELL_W + CELL_W / 2.f;
        float cy = GRID_START_Y + row * CELL_H + CELL_H / 2.f;

        const std::string &btn = gridButtons[i];
        std::string prefix = toLower(btn);
        bool active = isActive(btn);

        std::string key = prefix + (active ? activeSuffix : "-outline");
        if (active && textures.find(key) == textures.end())
        {
            key = prefix + "-filled";
            if (textures.find(key) == textures.end())
                key = prefix + "-pressed";
        }

        if (textures.find(key) != textures.end())
            drawSpriteCentered(key, cx, cy - 8.f, BTN_SCALE);
        else
        {
            text.setCharacterSize(20u);
            text.setFillColor(active ? sf::Color::White : sf::Color(180, 180, 180));
            text.setString(btn);
            auto bounds = text.getLocalBounds();
            text.setPosition({cx - bounds.size.x / 2.f, cy - bounds.size.y / 2.f - 8.f});
            window.draw(text);
        }

        text.setCharacterSize(11u);
        text.setFillColor(active ? sf::Color::White : sf::Color(100, 100, 100));
        text.setString(btn);
        auto lb = text.getLocalBounds();
        text.setPosition({cx - lb.size.x / 2.f, cy + 30.f});
        window.draw(text);
    }

    // --- D-pad widget ---
    float dpadCX = 150.f;
    float dpadCY = 340.f;
    float widgetScale = 0.85f;

    drawSpriteCentered("dpad-gate", dpadCX, dpadCY, widgetScale);

    if (isActive("DPad Up"))
        drawSpriteCentered("dpad-pressed-up", dpadCX, dpadCY, widgetScale);
    if (isActive("DPad Down"))
        drawSpriteCentered("dpad-pressed-down", dpadCX, dpadCY, widgetScale);
    if (isActive("DPad Left"))
        drawSpriteCentered("dpad-pressed-left", dpadCX, dpadCY, widgetScale);
    if (isActive("DPad Right"))
        drawSpriteCentered("dpad-pressed-right", dpadCX, dpadCY, widgetScale);

    text.setCharacterSize(11u);
    text.setFillColor(sf::Color(100, 100, 100));
    text.setString("D-Pad");
    auto dpb = text.getLocalBounds();
    text.setPosition({dpadCX - dpb.size.x / 2.f, dpadCY + 60.f});
    window.draw(text);

    // --- Joystick widgets ---
    auto drawStick = [&](float cx, float cy, float scale,
                         const std::string &gateKey, const std::string &stickKey,
                         const std::string &ribsKey,
                         const std::string &axisXBtn, const std::string &axisYBtn,
                         const std::string &l3Btn, const std::string &label) {
        drawSpriteCentered(gateKey, cx, cy, scale);

        float dx = 0.f, dy = 0.f;
        auto itX = settings.mappings.find(axisXBtn);
        if (itX != settings.mappings.end() && itX->second.type == InputType::GamepadAxis)
            dx = controller.getAxisValue(itX->second.code) / 32768.f;
        auto itY = settings.mappings.find(axisYBtn);
        if (itY != settings.mappings.end() && itY->second.type == InputType::GamepadAxis)
            dy = controller.getAxisValue(itY->second.code) / 32768.f;

        float maxDisp = 20.f * scale;
        float stickX = cx + dx * maxDisp;
        float stickY = cy + dy * maxDisp;

        bool l3Active = isActive(l3Btn);
        const std::string &sKey = (l3Active && textures.count(ribsKey)) ? ribsKey : stickKey;
        drawSpriteCentered(sKey, stickX, stickY, scale);

        text.setCharacterSize(11u);
        text.setFillColor(sf::Color(100, 100, 100));
        text.setString(label);
        auto b = text.getLocalBounds();
        text.setPosition({cx - b.size.x / 2.f, cy + 60.f});
        window.draw(text);
    };

    drawStick(400.f, 340.f, widgetScale,
              "lstick-gate", "lstick", "lstick-ribs",
              "LStick X", "LStick Y", "L3", "L Stick");

    drawStick(650.f, 340.f, widgetScale,
              "rstick-gate", "rstick", "rstick-ribs",
              "RStick X", "RStick Y", "R3", "R Stick");

    // Active style indicator
    text.setCharacterSize(11u);
    text.setFillColor(sf::Color(80, 80, 80));
    std::string styleStr = (settings.activeStyle == ActiveStyle::Pressed) ? "Style: Pressed" : "Style: Filled";
    text.setString(styleStr);
    text.setPosition({10.f, 580.f});
    window.draw(text);

    window.display();
}

void Display::renderSettings(const Settings &settings)
{
    window.clear(sf::Color(30, 30, 40));

    text.setCharacterSize(14u);
    text.setFillColor(sf::Color(150, 150, 150));
    text.setString("Tab: Main View");
    text.setPosition({10.f, 5.f});
    window.draw(text);

    text.setCharacterSize(22u);
    text.setFillColor(sf::Color::White);
    text.setString("Input Mappings");
    text.setPosition({ROW_X, 30.f});
    window.draw(text);

    // Active style toggle row
    {
        float rowY = ROW_START_Y - settingsScroll;
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
        float rowY = ROW_START_Y + (i + 1) * ROW_HEIGHT - settingsScroll;
        if (rowY + ROW_HEIGHT < ROW_START_Y || rowY > 600.f)
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
