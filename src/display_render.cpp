#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"
#include <cmath>
#include <algorithm>
#include <set>

static bool isMappingActive(const InputMapping &mapping, const Controller &controller, int deadzoneRaw = 16000)
{
    switch (mapping.type)
    {
    case InputType::Key:
        return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Scancode>(mapping.code));
    case InputType::GamepadButton:
        return controller.isButtonPressed(mapping.code);
    case InputType::GamepadAxis:
        return std::abs(controller.getAxisValue(mapping.code)) > deadzoneRaw;
    default:
        return false;
    }
}

static void drawButtonImage(Display &d, const std::map<std::string, sf::Texture> &textures,
                            sf::RenderWindow &window, sf::Text &text,
                            const std::string &btn, float cx, float cy,
                            bool active, sf::Color tint)
{
    std::string prefix = toLower(btn);
    std::string key = prefix + (active ? "-active" : "-inactive");

    auto it = textures.find(key);
    if (it != textures.end())
    {
        sf::Sprite sprite(it->second);
        auto size = it->second.getSize();
        sprite.setOrigin({size.x / 2.f, size.y / 2.f});
        sprite.setScale({BTN_SCALE, BTN_SCALE});
        sprite.setPosition({cx, cy - 8.f});
        sprite.setColor(tint);
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
    window.clear(parseHexColor(settings.bgColor));
    drawNavBar();
    float offy = navBarHeight();


    int deadzoneRaw = settings.deadzone * 32768 / 100;
    auto isActive = [&](const std::string &btn) -> bool {
        auto it = settings.mappings.find(btn);
        if (it == settings.mappings.end() || it->second.type == InputType::None)
            return false;
        return isMappingActive(it->second, controller, deadzoneRaw);
    };

    static const std::vector<std::string> gridButtons = {
        "A", "B", "X", "Y", "Start",
        "LB", "RB", "Select"};

    for (const auto &btn : gridButtons)
    {
        auto it = settings.layout.find(btn);
        if (it == settings.layout.end())
            continue;
        float cx = it->second.x;
        float cy = it->second.y + offy;
        bool active = isActive(btn);
        std::string elem = elementForButton(btn);
        sf::Color tint = parseHexColor(active ? settings.getActiveColor(elem) : settings.getInactiveColor(elem));
        drawButtonImage(*this, textures, window, text, btn, cx, cy, active, tint);
    }

    // Triggers (analog fill)
    auto drawTrigger = [&](const std::string &btn, bool fromLeft) {
        auto layoutIt = settings.layout.find(btn);
        if (layoutIt == settings.layout.end())
            return;
        float cx = layoutIt->second.x;
        float cy = layoutIt->second.y + offy;

        std::string prefix = toLower(btn);
        sf::Color inactiveTint = parseHexColor(settings.getInactiveColor(btn));
        sf::Color activeTint = parseHexColor(settings.getActiveColor(btn));
        drawSpriteTinted(prefix + "-inactive", cx, cy - 8.f, BTN_SCALE, inactiveTint);

        float fill = 0.f;
        auto mapIt = settings.mappings.find(btn);
        if (mapIt != settings.mappings.end() && mapIt->second.type == InputType::GamepadAxis)
            fill = controller.getAxisValue(mapIt->second.code) / 32768.f;

        if (fill > 0.f)
            drawSpritePartialFill(prefix + "-active", cx, cy - 8.f, BTN_SCALE, fill, fromLeft, activeTint);
    };
    drawTrigger("LT", false);
    drawTrigger("RT", true);

    // D-pad
    {
        auto it = settings.layout.find("DPad");
        if (it != settings.layout.end())
        {
            float cx = it->second.x;
            float cy = it->second.y + offy;

            sf::Color dpadInactive = parseHexColor(settings.getInactiveColor("DPad"));
            sf::Color dpadActive = parseHexColor(settings.getActiveColor("DPad"));
            drawSpriteTinted("dpad-gate-inactive", cx, cy, WIDGET_SCALE, dpadInactive);
            if (isActive("DPad Up"))
                drawSpriteTinted("dpad-active-up", cx, cy, WIDGET_SCALE, dpadActive);
            if (isActive("DPad Down"))
                drawSpriteTinted("dpad-active-down", cx, cy, WIDGET_SCALE, dpadActive);
            if (isActive("DPad Left"))
                drawSpriteTinted("dpad-active-left", cx, cy, WIDGET_SCALE, dpadActive);
            if (isActive("DPad Right"))
                drawSpriteTinted("dpad-active-right", cx, cy, WIDGET_SCALE, dpadActive);
        }
    }

    // Joysticks
    auto drawStick = [&](const std::string &layoutKey,
                         const std::string &baseKey,
                         const std::string &axisXBtn, const std::string &axisYBtn,
                         const std::string &l3Btn) {
        auto it = settings.layout.find(layoutKey);
        if (it == settings.layout.end())
            return;

        float cx = it->second.x;
        float cy = it->second.y + offy;

        sf::Color stickInactive = parseHexColor(settings.getInactiveColor(layoutKey));
        sf::Color stickActive = parseHexColor(settings.getActiveColor(layoutKey));

        float dx = 0.f, dy = 0.f;
        auto itX = settings.mappings.find(axisXBtn);
        if (itX != settings.mappings.end() && itX->second.type == InputType::GamepadAxis)
            dx = controller.getAxisValue(itX->second.code) / 32768.f;
        auto itY = settings.mappings.find(axisYBtn);
        if (itY != settings.mappings.end() && itY->second.type == InputType::GamepadAxis)
            dy = controller.getAxisValue(itY->second.code) / 32768.f;

        drawSpriteTinted(baseKey + "-gate-inactive", cx, cy, WIDGET_SCALE, stickInactive);

        float maxDisp = 20.f * WIDGET_SCALE;
        float stickX = cx + dx * maxDisp;
        float stickY = cy + dy * maxDisp;

        bool l3Active = isActive(l3Btn);
        std::string sKey;
        if (l3Active)
            sKey = baseKey + "-ribs-active";
        else
            sKey = baseKey + "-inactive";

        if (!textures.count(sKey))
            sKey = baseKey + "-inactive";

        bool stickMoved = (std::abs(dx) > 0.1f || std::abs(dy) > 0.1f || l3Active);
        drawSpriteTinted(sKey, stickX, stickY, WIDGET_SCALE, stickMoved ? stickActive : stickInactive);
    };

    drawStick("LStick", "lstick", "LStick X", "LStick Y", "L3");
    drawStick("RStick", "rstick", "RStick X", "RStick Y", "R3");

    window.display();
}

static std::string axisDirectionName(const std::string &btn, const Controller &controller, const InputMapping &mapping)
{
    int16_t val = controller.getAxisValue(mapping.code);
    if (btn == "LStick X")
        return val > 0 ? "LStick Right" : "LStick Left";
    if (btn == "LStick Y")
        return val > 0 ? "LStick Down" : "LStick Up";
    if (btn == "RStick X")
        return val > 0 ? "RStick Right" : "RStick Left";
    if (btn == "RStick Y")
        return val > 0 ? "RStick Down" : "RStick Up";
    return btn;
}

void Display::updateHistory(const Settings &settings, const Controller &controller)
{
    int deadzoneRaw = settings.deadzone * 32768 / 100;

    std::set<std::string> currentActive;
    for (const auto &btn : GAMEPAD_BUTTONS)
    {
        auto it = settings.mappings.find(btn);
        if (it == settings.mappings.end() || it->second.type == InputType::None)
            continue;
        if (!isMappingActive(it->second, controller, deadzoneRaw))
            continue;
        bool isStickAxis = (btn == "LStick X" || btn == "LStick Y" || btn == "RStick X" || btn == "RStick Y");
        if (isStickAxis && !settings.trackSticks)
            continue;
        if (isStickAxis && it->second.type == InputType::GamepadAxis)
            currentActive.insert(axisDirectionName(btn, controller, it->second));
        else
            currentActive.insert(btn);
    }

    std::vector<std::string> newlyPressed;
    for (const auto &btn : currentActive)
    {
        if (prevActiveButtons.find(btn) == prevActiveButtons.end())
            newlyPressed.push_back(btn);
    }

    if (!newlyPressed.empty())
    {
        if (!inputHistory.empty() && framesSinceLastEntry <= settings.inputGroupFrames)
        {
            for (const auto &btn : newlyPressed)
            {
                auto &existing = inputHistory.front().buttons;
                if (std::find(existing.begin(), existing.end(), btn) == existing.end())
                    existing.push_back(btn);
            }
        }
        else
        {
            HistoryEntry entry;
            entry.buttons = newlyPressed;
            entry.frameGap = framesSinceLastEntry;
            inputHistory.push_front(entry);
            if (inputHistory.size() > 10)
                inputHistory.pop_back();
        }
        framesSinceLastEntry = 0;
    }
    else
    {
        framesSinceLastEntry++;
    }

    prevActiveButtons = currentActive;
}

static bool isDpadButton(const std::string &btn)
{
    return btn == "DPad Up" || btn == "DPad Down" || btn == "DPad Left" || btn == "DPad Right";
}

static bool isStickButton(const std::string &btn)
{
    return btn == "L3" || btn == "R3" ||
           btn.rfind("LStick ", 0) == 0 || btn.rfind("RStick ", 0) == 0;
}

static std::string stickElement(const std::string &btn)
{
    if (btn == "L3" || btn.rfind("LStick ", 0) == 0)
        return "LStick";
    return "RStick";
}

struct HistoryIcon {
    std::string textureKey;
    std::string overlayKey;
    std::string element;
    std::string label;
};

void Display::renderHistory(const Settings &settings)
{
    if (!historyOpen || !historyWindow.isOpen())
        return;

    while (const std::optional event = historyWindow.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            historyWindow.close();
            historyOpen = false;
            return;
        }
    }

    historyWindow.clear(parseHexColor(settings.bgColor));

    sf::Text title(font, "Input History", 20u);
    title.setFillColor(sf::Color::White);
    title.setPosition({15.f, 8.f});
    historyWindow.draw(title);

    const float ICON_SCALE = 0.35f;
    const float MAX_ROW_W = 370.f;
    const float ROW_PAD = 25.f;
    const float LINE_H = 40.f;
    const float ENTRY_GAP = 5.f;

    float rowY = 45.f;
    for (size_t i = 0; i < inputHistory.size(); i++)
    {
        std::vector<HistoryIcon> icons;
        std::set<std::string> sticksSeen;

        for (const auto &btn : inputHistory[i].buttons)
        {
            if (isDpadButton(btn))
            {
                std::string dirKey;
                if (btn == "DPad Up") dirKey = "dpad-active-up";
                else if (btn == "DPad Down") dirKey = "dpad-active-down";
                else if (btn == "DPad Left") dirKey = "dpad-active-left";
                else if (btn == "DPad Right") dirKey = "dpad-active-right";
                icons.push_back({"dpad-gate-inactive", dirKey, "DPad", ""});
                continue;
            }

            if (isStickButton(btn))
            {
                std::string elem = stickElement(btn);
                bool isL3R3 = (btn == "L3" || btn == "R3");

                if (sticksSeen.insert(elem).second)
                {
                    std::string base = toLower(elem);
                    std::string key;
                    if (isL3R3)
                        key = base + "-ribs-active";
                    else
                        key = base + "-active";
                    if (textures.find(key) == textures.end())
                        key = base + "-inactive";
                    icons.push_back({key, "", elem, btn});
                }
                else
                {
                    if (isL3R3)
                    {
                        for (auto &ic : icons)
                        {
                            if (ic.element == elem)
                            {
                                std::string base = toLower(elem);
                                std::string ribsKey = base + "-ribs-active";
                                if (textures.find(ribsKey) != textures.end())
                                    ic.textureKey = ribsKey;
                                if (!ic.label.empty())
                                    ic.label += " + " + btn;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (auto &ic : icons)
                        {
                            if (ic.element == elem && !ic.label.empty())
                            {
                                ic.label += " + " + btn;
                                break;
                            }
                        }
                    }
                }
                continue;
            }

            std::string prefix = toLower(btn);
            icons.push_back({prefix + "-active", "", btn, ""});
        }

        float totalH = LINE_H;
        {
            float x = 0.f;
            for (const auto &ic : icons)
            {
                float w = 30.f;
                auto texIt = textures.find(ic.textureKey);
                if (texIt != textures.end())
                    w = texIt->second.getSize().x * ICON_SCALE;
                if (!ic.label.empty())
                    w += 5.f + ic.label.size() * 7.f;
                if (x + w > MAX_ROW_W - ROW_PAD * 2.f && x > 0.f)
                {
                    totalH += LINE_H;
                    x = 0.f;
                }
                x += w + 8.f;
            }
        }

        sf::RectangleShape rowBg(sf::Vector2f(MAX_ROW_W, totalH));
        rowBg.setPosition({15.f, rowY});
        rowBg.setFillColor(sf::Color(45, 45, 60));
        rowBg.setOutlineColor(sf::Color(70, 70, 90));
        rowBg.setOutlineThickness(0.5f);
        historyWindow.draw(rowBg);

        float iconX = ROW_PAD;
        float lineY = rowY;

        for (const auto &ic : icons)
        {
            auto texIt = textures.find(ic.textureKey);
            float itemW = 30.f;
            if (texIt != textures.end())
                itemW = texIt->second.getSize().x * ICON_SCALE;
            if (!ic.label.empty())
                itemW += 5.f + ic.label.size() * 7.f;

            if (iconX - ROW_PAD + itemW > MAX_ROW_W - ROW_PAD * 2.f && iconX > ROW_PAD)
            {
                lineY += LINE_H;
                iconX = ROW_PAD;
            }

            if (texIt != textures.end())
            {
                sf::Color inactiveTint = parseHexColor(settings.getInactiveColor(ic.element));
                sf::Color activeTint = parseHexColor(settings.getActiveColor(ic.element));
                bool hasOverlay = !ic.overlayKey.empty();
                sf::Color tint = hasOverlay ? inactiveTint : activeTint;

                sf::Sprite sprite(texIt->second);
                auto size = texIt->second.getSize();
                sprite.setOrigin({size.x / 2.f, size.y / 2.f});
                sprite.setScale({ICON_SCALE, ICON_SCALE});
                float spriteX = iconX + (size.x * ICON_SCALE) / 2.f;
                float spriteY = lineY + LINE_H / 2.f;
                sprite.setPosition({spriteX, spriteY});

                if (shaderLoaded && tint != sf::Color::White)
                {
                    sf::Glsl::Vec4 tintVec(tint.r / 255.f, tint.g / 255.f, tint.b / 255.f, tint.a / 255.f);
                    tintShader.setUniform("tintColor", tintVec);
                    tintShader.setUniform("texture", sf::Shader::CurrentTexture);
                    historyWindow.draw(sprite, sf::RenderStates(&tintShader));
                }
                else
                {
                    historyWindow.draw(sprite);
                }

                if (hasOverlay)
                {
                    auto ovIt = textures.find(ic.overlayKey);
                    if (ovIt != textures.end())
                    {
                        sf::Sprite ovSprite(ovIt->second);
                        auto ovSize = ovIt->second.getSize();
                        ovSprite.setOrigin({ovSize.x / 2.f, ovSize.y / 2.f});
                        ovSprite.setScale({ICON_SCALE, ICON_SCALE});
                        ovSprite.setPosition({spriteX, spriteY});

                        if (shaderLoaded && activeTint != sf::Color::White)
                        {
                            sf::Glsl::Vec4 tintVec(activeTint.r / 255.f, activeTint.g / 255.f, activeTint.b / 255.f, activeTint.a / 255.f);
                            tintShader.setUniform("tintColor", tintVec);
                            tintShader.setUniform("texture", sf::Shader::CurrentTexture);
                            historyWindow.draw(ovSprite, sf::RenderStates(&tintShader));
                        }
                        else
                        {
                            historyWindow.draw(ovSprite);
                        }
                    }
                }

                iconX += size.x * ICON_SCALE;

                if (!ic.label.empty())
                {
                    iconX += 5.f;
                    sf::Text lbl(font, ic.label, 11u);
                    lbl.setFillColor(sf::Color(200, 200, 220));
                    lbl.setPosition({iconX, lineY + LINE_H / 2.f - 7.f});
                    historyWindow.draw(lbl);
                    iconX += lbl.getLocalBounds().size.x;
                }

                iconX += 8.f;
            }
            else
            {
                sf::Text label(font, ic.label.empty() ? ic.element : ic.label, 14u);
                label.setFillColor(sf::Color::White);
                label.setPosition({iconX, lineY + LINE_H / 2.f - 9.f});
                historyWindow.draw(label);
                iconX += label.getLocalBounds().size.x + 10.f;
            }
        }

        if (settings.trackFrames && i > 0 && inputHistory[i - 1].frameGap > 0)
        {
            sf::Text gap(font, std::to_string(inputHistory[i - 1].frameGap) + "f", 10u);
            gap.setFillColor(sf::Color(120, 120, 140));
            float gapW = gap.getLocalBounds().size.x;
            gap.setPosition({15.f + MAX_ROW_W - gapW - 8.f, rowY + totalH - 16.f});
            historyWindow.draw(gap);
        }

        rowY += totalH + ENTRY_GAP;
    }

    historyWindow.display();
}
