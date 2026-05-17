#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"
#include <algorithm>

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
                if (editingBgColor)
                {
                    editingBgColor = false;
                    continue;
                }
                if (editingColorIndex >= 0)
                {
                    editingColorIndex = -1;
                    continue;
                }
                window.close();
                return;
            }

            if (editingColorIndex >= 0 && viewMode == ViewMode::Settings)
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
                {
                    if (colorInput.size() == 6)
                    {
                        std::string elem = elementForButton(GAMEPAD_BUTTONS[editingColorIndex]);
                        if (editingColorIsActive)
                            settings.elementActiveColors[elem] = colorInput;
                        else
                            settings.elementInactiveColors[elem] = colorInput;
                    }
                    editingColorIndex = -1;
                    continue;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace)
                {
                    if (!colorInput.empty())
                        colorInput.pop_back();
                    continue;
                }
                continue;
            }

            if (editingBgColor && viewMode == ViewMode::Settings)
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
                {
                    if (bgColorInput.size() == 6)
                        settings.bgColor = bgColorInput;
                    editingBgColor = false;
                    continue;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace)
                {
                    if (!bgColorInput.empty())
                        bgColorInput.pop_back();
                    continue;
                }
                continue;
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

        if (const auto *textEvent = event->getIf<sf::Event::TextEntered>())
        {
            if (editingColorIndex >= 0 && viewMode == ViewMode::Settings)
            {
                char c = static_cast<char>(textEvent->unicode);
                if (colorInput.size() < 6 &&
                    ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
                {
                    colorInput += static_cast<char>(std::toupper(c));
                }
                continue;
            }
            if (editingBgColor && viewMode == ViewMode::Settings)
            {
                char c = static_cast<char>(textEvent->unicode);
                if (bgColorInput.size() < 6 &&
                    ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
                {
                    bgColorInput += static_cast<char>(std::toupper(c));
                }
                continue;
            }
        }

        if (const auto *scroll = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            if (viewMode == ViewMode::Settings)
            {
                settingsScroll -= scroll->delta * 30.f;
                float maxScroll = std::max(0.f, ROW_START_Y + (float)(GAMEPAD_BUTTONS.size() + 2) * ROW_HEIGHT - 600.f + navBarHeight());
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

                    float bgColorY = ROW_START_Y + ROW_HEIGHT + offy - settingsScroll;
                    if (mx >= ROW_X && mx <= ROW_X + ROW_WIDTH && my >= bgColorY && my <= bgColorY + ROW_HEIGHT - 2.f)
                    {
                        editingBgColor = true;
                        bgColorInput = settings.bgColor;
                        detectingIndex = -1;
                        continue;
                    }

                    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
                    {
                        float rowY = ROW_START_Y + (i + 2) * ROW_HEIGHT + offy - settingsScroll;
                        if (my >= rowY && my <= rowY + ROW_HEIGHT - 2.f)
                        {
                            float colorX = ROW_X + 480.f;
                            float activeX = colorX + 100.f;

                            if (mx >= activeX && mx <= activeX + 90.f)
                            {
                                std::string elem = elementForButton(GAMEPAD_BUTTONS[i]);
                                editingColorIndex = i;
                                editingColorIsActive = true;
                                colorInput = settings.getActiveColor(elem);
                                editingBgColor = false;
                                detectingIndex = -1;
                                break;
                            }
                            if (mx >= colorX && mx <= colorX + 90.f)
                            {
                                std::string elem = elementForButton(GAMEPAD_BUTTONS[i]);
                                editingColorIndex = i;
                                editingColorIsActive = false;
                                colorInput = settings.getInactiveColor(elem);
                                editingBgColor = false;
                                detectingIndex = -1;
                                break;
                            }
                            if (mx >= ROW_X && mx <= ROW_X + 470.f)
                            {
                                detectingIndex = i;
                                editingColorIndex = -1;
                                break;
                            }
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
