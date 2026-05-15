#include <SFML/Graphics.hpp>
#include "display.h"
#include <iostream>

static const float ROW_HEIGHT = 32.f;
static const float ROW_START_Y = 70.f;
static const float ROW_X = 50.f;
static const float ROW_WIDTH = 700.f;

Display::Display()
    : window(sf::VideoMode({800u, 600u}), "Input Viewer"),
      font(),
      text(font, "", 18u)
{
    if (!font.openFromFile("resources/OpenDyslexic-Regular.otf"))
    {
        std::cerr << "Failed to load font from resources/OpenDyslexic-Regular.otf" << std::endl;
    }
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
                float maxScroll = std::max(0.f, ROW_START_Y + (float)GAMEPAD_BUTTONS.size() * ROW_HEIGHT - 600.f);
                if (settingsScroll < 0.f) settingsScroll = 0.f;
                if (settingsScroll > maxScroll) settingsScroll = maxScroll;
            }
        }

        if (const auto *mouseBtn = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseBtn->button == sf::Mouse::Button::Left && viewMode == ViewMode::Settings)
            {
                float mx = static_cast<float>(mouseBtn->position.x);
                float my = static_cast<float>(mouseBtn->position.y);
                for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
                {
                    float rowY = ROW_START_Y + i * ROW_HEIGHT - settingsScroll;
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

void Display::render(const Settings &settings, const Controller &controller)
{
    window.clear(sf::Color::Black);

    text.setCharacterSize(14u);
    text.setFillColor(sf::Color(150, 150, 150));
    text.setString("Tab: Settings");
    text.setPosition({10.f, 5.f});
    window.draw(text);

    static const int COLS = 4;
    static const float PAD = 40.f;
    static const float GAP = 10.f;
    static const float COL_W = (800.f - 2 * PAD - (COLS - 1) * GAP) / COLS;
    static const float CELL_H = 70.f;
    static const float START_Y = 60.f;

    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
    {
        int col = i % COLS;
        int row = i / COLS;
        float x = PAD + col * (COL_W + GAP);
        float y = START_Y + row * (CELL_H + GAP);

        bool active = false;
        auto it = settings.mappings.find(GAMEPAD_BUTTONS[i]);
        if (it != settings.mappings.end() && it->second.type != InputType::None)
            active = isMappingActive(it->second, controller);

        sf::RectangleShape cell(sf::Vector2f(COL_W, CELL_H));
        cell.setPosition(sf::Vector2f(x, y));
        cell.setFillColor(active ? sf::Color(40, 180, 40) : sf::Color(50, 50, 60));
        cell.setOutlineColor(active ? sf::Color(80, 255, 80) : sf::Color(80, 80, 100));
        cell.setOutlineThickness(active ? 2.f : 1.f);
        window.draw(cell);

        text.setCharacterSize(20u);
        text.setFillColor(active ? sf::Color::White : sf::Color(180, 180, 180));
        text.setString(GAMEPAD_BUTTONS[i]);
        auto bounds = text.getLocalBounds();
        text.setPosition(sf::Vector2f(x + (COL_W - bounds.size.x) / 2.f, y + 10.f));
        window.draw(text);

        if (it != settings.mappings.end() && it->second.type != InputType::None)
        {
            text.setCharacterSize(12u);
            text.setFillColor(active ? sf::Color(220, 255, 220) : sf::Color(120, 120, 120));

            if (it->second.type == InputType::GamepadAxis)
            {
                int16_t val = controller.getAxisValue(it->second.code);
                text.setString(it->second.name + ": " + std::to_string(val));
            }
            else
            {
                text.setString(it->second.name);
            }

            auto sub = text.getLocalBounds();
            text.setPosition(sf::Vector2f(x + (COL_W - sub.size.x) / 2.f, y + 42.f));
            window.draw(text);
        }
    }

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

    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
    {
        float rowY = ROW_START_Y + i * ROW_HEIGHT - settingsScroll;
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
