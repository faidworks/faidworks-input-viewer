#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"
#include <algorithm>

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

    // Font selection
    {
        float rowY = baseY + 2.f * ROW_HEIGHT - settingsScroll;
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
            text.setString("Font");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            std::string fontName = (fontIndex >= 0 && fontIndex < (int)availableFonts.size())
                                       ? availableFonts[fontIndex].first
                                       : "Unknown";
            text.setFillColor(sf::Color(150, 255, 150));
            text.setString(fontName + "  (click to change)");
            text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
            window.draw(text);
        }
    }

    // FPS limit
    {
        float rowY = baseY + 3.f * ROW_HEIGHT - settingsScroll;
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
            text.setString("FPS Limit");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            std::string val = (settings.fpsLimit <= 0) ? "Unlimited" : std::to_string(settings.fpsLimit);
            text.setFillColor(sf::Color(150, 255, 150));
            text.setString(val + "  (click to cycle)");
            text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
            window.draw(text);
        }
    }

    // Deadzone slider
    {
        float rowY = baseY + 4.f * ROW_HEIGHT - settingsScroll;
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
            text.setString("Deadzone");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            float sliderX = ROW_X + 200.f;
            float sliderW = 350.f;
            float sliderY = rowY + ROW_HEIGHT / 2.f;

            sf::RectangleShape track(sf::Vector2f(sliderW, 4.f));
            track.setPosition(sf::Vector2f(sliderX, sliderY - 2.f));
            track.setFillColor(sf::Color(80, 80, 100));
            window.draw(track);

            float t = settings.deadzone / 100.f;
            sf::RectangleShape fill(sf::Vector2f(sliderW * t, 4.f));
            fill.setPosition(sf::Vector2f(sliderX, sliderY - 2.f));
            fill.setFillColor(sf::Color(100, 200, 100));
            window.draw(fill);

            sf::CircleShape knob(7.f);
            knob.setOrigin({7.f, 7.f});
            knob.setPosition({sliderX + sliderW * t, sliderY});
            knob.setFillColor(sf::Color(150, 255, 150));
            window.draw(knob);

            text.setCharacterSize(14u);
            text.setFillColor(sf::Color(150, 255, 150));
            text.setString(std::to_string(settings.deadzone) + "%");
            text.setPosition(sf::Vector2f(sliderX + sliderW + 15.f, rowY + 6.f));
            window.draw(text);
        }
    }

    // Input group frames slider
    {
        float rowY = baseY + 5.f * ROW_HEIGHT - settingsScroll;
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
            text.setString("Input Group");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            float sliderX = ROW_X + 200.f;
            float sliderW = 350.f;
            float sliderY = rowY + ROW_HEIGHT / 2.f;

            sf::RectangleShape track(sf::Vector2f(sliderW, 4.f));
            track.setPosition(sf::Vector2f(sliderX, sliderY - 2.f));
            track.setFillColor(sf::Color(80, 80, 100));
            window.draw(track);

            float t = settings.inputGroupFrames / 5.f;
            sf::RectangleShape fill(sf::Vector2f(sliderW * t, 4.f));
            fill.setPosition(sf::Vector2f(sliderX, sliderY - 2.f));
            fill.setFillColor(sf::Color(100, 200, 100));
            window.draw(fill);

            sf::CircleShape knob(7.f);
            knob.setOrigin({7.f, 7.f});
            knob.setPosition({sliderX + sliderW * t, sliderY});
            knob.setFillColor(sf::Color(150, 255, 150));
            window.draw(knob);

            text.setCharacterSize(14u);
            text.setFillColor(sf::Color(150, 255, 150));
            text.setString(std::to_string(settings.inputGroupFrames) + " frames");
            text.setPosition(sf::Vector2f(sliderX + sliderW + 15.f, rowY + 6.f));
            window.draw(text);
        }
    }

    // History key binding
    {
        float rowY = baseY + 6.f * ROW_HEIGHT - settingsScroll;
        if (rowY + ROW_HEIGHT >= baseY && rowY <= 600.f)
        {
            sf::RectangleShape row(sf::Vector2f(ROW_WIDTH, ROW_HEIGHT - 2.f));
            row.setPosition(sf::Vector2f(ROW_X, rowY));
            row.setFillColor(detectingHistoryKey ? sf::Color(80, 80, 140) : sf::Color(60, 50, 80));
            row.setOutlineColor(sf::Color(100, 80, 140));
            row.setOutlineThickness(1.f);
            window.draw(row);

            text.setCharacterSize(16u);
            text.setFillColor(sf::Color(200, 180, 255));
            text.setString("History Key");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            std::string val;
            if (detectingHistoryKey)
                val = ">> Press a key... <<";
            else
                val = settings.historyKeyName + "  (click to rebind)";

            text.setFillColor(detectingHistoryKey ? sf::Color::Yellow : sf::Color(150, 255, 150));
            text.setString(val);
            text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
            window.draw(text);
        }
    }

    // Track Frames toggle
    {
        float rowY = baseY + 7.f * ROW_HEIGHT - settingsScroll;
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
            text.setString("Track Frames");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            std::string val = settings.trackFrames ? "On" : "Off";
            text.setFillColor(sf::Color(150, 255, 150));
            text.setString(val + "  (click to toggle)");
            text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
            window.draw(text);
        }
    }

    // Track Sticks toggle
    {
        float rowY = baseY + 8.f * ROW_HEIGHT - settingsScroll;
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
            text.setString("Track Sticks");
            text.setPosition(sf::Vector2f(ROW_X + 10.f, rowY + 5.f));
            window.draw(text);

            std::string val = settings.trackSticks ? "On" : "Off";
            text.setFillColor(sf::Color(150, 255, 150));
            text.setString(val + "  (click to toggle)");
            text.setPosition(sf::Vector2f(ROW_X + 200.f, rowY + 5.f));
            window.draw(text);
        }
    }

    for (int i = 0; i < (int)GAMEPAD_BUTTONS.size(); i++)
    {
        float rowY = baseY + (i + 9) * ROW_HEIGHT - settingsScroll;
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

        // Color swatches to the right
        std::string elem = elementForButton(GAMEPAD_BUTTONS[i]);
        float colorX = ROW_X + 480.f;

        // Inactive color swatch
        {
            bool editing = (editingColorIndex == i && !editingColorIsActive);
            std::string hexVal = editing ? colorInput : settings.getInactiveColor(elem);
            sf::Color swatchColor = parseHexColor(hexVal);

            sf::RectangleShape swatch(sf::Vector2f(14.f, 14.f));
            swatch.setPosition(sf::Vector2f(colorX, rowY + 7.f));
            swatch.setFillColor(swatchColor);
            swatch.setOutlineColor(editing ? sf::Color::Yellow : sf::Color(80, 80, 100));
            swatch.setOutlineThickness(1.f);
            window.draw(swatch);

            text.setCharacterSize(11u);
            text.setFillColor(editing ? sf::Color::Yellow : sf::Color(160, 160, 180));
            text.setString(editing ? hexVal + "_" : hexVal);
            text.setPosition(sf::Vector2f(colorX + 18.f, rowY + 8.f));
            window.draw(text);
        }

        // Active color swatch
        {
            float activeX = colorX + 100.f;
            bool editing = (editingColorIndex == i && editingColorIsActive);
            std::string hexVal = editing ? colorInput : settings.getActiveColor(elem);
            sf::Color swatchColor = parseHexColor(hexVal);

            sf::RectangleShape swatch(sf::Vector2f(14.f, 14.f));
            swatch.setPosition(sf::Vector2f(activeX, rowY + 7.f));
            swatch.setFillColor(swatchColor);
            swatch.setOutlineColor(editing ? sf::Color::Yellow : sf::Color(80, 80, 100));
            swatch.setOutlineThickness(1.f);
            window.draw(swatch);

            text.setCharacterSize(11u);
            text.setFillColor(editing ? sf::Color::Yellow : sf::Color(160, 160, 180));
            text.setString(editing ? hexVal + "_" : hexVal);
            text.setPosition(sf::Vector2f(activeX + 18.f, rowY + 8.f));
            window.draw(text);
        }
    }

    if (fontPickerOpen)
    {
        sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        const float PX = 100.f, PY = 50.f, PW = 600.f, PH = 470.f;
        const float TITLE_H = 40.f, PROW_H = 32.f;
        float listY = PY + TITLE_H;
        float listH = PH - TITLE_H;

        sf::RectangleShape dialog(sf::Vector2f(PW, PH));
        dialog.setPosition({PX, PY});
        dialog.setFillColor(sf::Color(35, 35, 50));
        dialog.setOutlineColor(sf::Color(100, 80, 140));
        dialog.setOutlineThickness(2.f);
        window.draw(dialog);

        text.setCharacterSize(18u);
        text.setFillColor(sf::Color::White);
        text.setString("Select Font");
        text.setPosition({PX + 15.f, PY + 8.f});
        window.draw(text);

        sf::RectangleShape sep(sf::Vector2f(PW - 4.f, 1.f));
        sep.setPosition({PX + 2.f, listY});
        sep.setFillColor(sf::Color(100, 80, 140));
        window.draw(sep);

        int firstVis = std::max(0, (int)(fontPickerScroll / PROW_H));
        int lastVis = std::min((int)availableFonts.size() - 1, firstVis + (int)(listH / PROW_H) + 1);

        for (auto it = fontPreviewCache.begin(); it != fontPreviewCache.end();)
        {
            if (it->first < firstVis - 5 || it->first > lastVis + 5)
                it = fontPreviewCache.erase(it);
            else
                ++it;
        }

        for (int i = firstVis; i <= lastVis; i++)
        {
            float rowY = listY + i * PROW_H - fontPickerScroll;
            if (rowY + PROW_H < listY || rowY > PY + PH)
                continue;

            bool selected = (i == fontIndex);

            sf::RectangleShape row(sf::Vector2f(PW - 4.f, PROW_H - 2.f));
            row.setPosition({PX + 2.f, rowY});
            row.setFillColor(selected ? sf::Color(80, 60, 120) : sf::Color(45, 45, 60));
            row.setOutlineColor(sf::Color(70, 70, 90));
            row.setOutlineThickness(0.5f);
            window.draw(row);

            auto cacheIt = fontPreviewCache.find(i);
            if (cacheIt == fontPreviewCache.end())
            {
                sf::Font previewFont;
                if (previewFont.openFromFile(availableFonts[i].second))
                    cacheIt = fontPreviewCache.emplace(i, std::move(previewFont)).first;
            }

            if (cacheIt != fontPreviewCache.end())
            {
                sf::Text previewText(cacheIt->second, availableFonts[i].first, 16u);
                previewText.setFillColor(selected ? sf::Color(200, 255, 200) : sf::Color(200, 200, 220));
                previewText.setPosition({PX + 15.f, rowY + 5.f});
                window.draw(previewText);
            }
            else
            {
                text.setCharacterSize(16u);
                text.setFillColor(selected ? sf::Color(200, 255, 200) : sf::Color(200, 200, 220));
                text.setString(availableFonts[i].first);
                text.setPosition({PX + 15.f, rowY + 5.f});
                window.draw(text);
            }
        }
    }

    window.display();
}
