#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"

void Display::renderSettings(const Settings &settings)
{
    window.clear(sf::Color(30, 30, 40));
    drawNavBar();
    float offy = navBarHeight();


    text.setCharacterSize(22u);
    text.setFillColor(sf::Color::White);
    text.setString("Input Mappings");
    text.setPosition({ROW_X, 30.f + offy});
    window.draw(text);

    float baseY = ROW_START_Y + offy;

    // Active style toggle
    {
        float rowY = baseY - settingsScroll;
        if (rowY + ROW_HEIGHT >= baseY && rowY <= 600.f)
        {
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
    }

    // Background color
    {
        float rowY = baseY + ROW_HEIGHT - settingsScroll;
        if (rowY + ROW_HEIGHT >= baseY && rowY <= 600.f)
        {
            sf::RectangleShape row(sf::Vector2f(ROW_WIDTH, ROW_HEIGHT - 2.f));
            row.setPosition(sf::Vector2f(ROW_X, rowY));
            row.setFillColor(editingBgColor ? sf::Color(80, 80, 140) : sf::Color(60, 50, 80));
            row.setOutlineColor(sf::Color(100, 80, 140));
            row.setOutlineThickness(1.f);
            window.draw(row);

            text.setCharacterSize(16u);
            text.setFillColor(sf::Color(200, 180, 255));
            text.setString("Background");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            sf::Color previewColor = parseHexColor(editingBgColor ? bgColorInput : settings.bgColor);
            sf::RectangleShape swatch(sf::Vector2f(20.f, 20.f));
            swatch.setPosition(sf::Vector2f(ROW_X + 170.f, rowY + 4.f));
            swatch.setFillColor(previewColor);
            swatch.setOutlineColor(sf::Color(100, 100, 120));
            swatch.setOutlineThickness(1.f);
            window.draw(swatch);

            std::string display;
            if (editingBgColor)
                display = "#" + bgColorInput + "_";
            else
                display = "#" + settings.bgColor + "  (click to edit)";

            text.setFillColor(editingBgColor ? sf::Color::Yellow : sf::Color(150, 255, 150));
            text.setString(display);
            text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
            window.draw(text);
        }
    }

    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
    {
        float rowY = baseY + (i + 2) * ROW_HEIGHT - settingsScroll;
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
