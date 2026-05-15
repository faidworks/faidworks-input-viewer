#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"
#include <cmath>

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

static void drawButtonImage(Display &d, const std::map<std::string, sf::Texture> &textures,
                            sf::RenderWindow &window, sf::Text &text,
                            const std::string &btn, float cx, float cy,
                            bool active, const std::string &inactiveSuffix)
{
    std::string prefix = toLower(btn);

    std::string key = prefix + (active ? "-pressed" : inactiveSuffix);
    if (textures.find(key) == textures.end())
        key = prefix + "-filled";
    if (textures.find(key) == textures.end())
        key = prefix + "-outline";

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
    window.clear(parseHexColor(settings.bgColor));
    drawNavBar();
    float offy = navBarHeight();


    auto isActive = [&](const std::string &btn) -> bool {
        auto it = settings.mappings.find(btn);
        if (it == settings.mappings.end() || it->second.type == InputType::None)
            return false;
        return isMappingActive(it->second, controller);
    };

    std::string inactiveSuffix = (settings.activeStyle == ActiveStyle::Filled) ? "-filled" : "-outline";

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
        drawButtonImage(*this, textures, window, text, btn, cx, cy, isActive(btn), inactiveSuffix);
    }

    bool filled = (settings.activeStyle == ActiveStyle::Filled);

    // Triggers (analog fill)
    auto drawTrigger = [&](const std::string &btn, bool fromLeft) {
        auto layoutIt = settings.layout.find(btn);
        if (layoutIt == settings.layout.end())
            return;
        float cx = layoutIt->second.x;
        float cy = layoutIt->second.y + offy;

        std::string prefix = toLower(btn);
        drawSpriteCentered(prefix + (filled ? "-filled" : "-outline"), cx, cy - 8.f, BTN_SCALE);

        float fill = 0.f;
        auto mapIt = settings.mappings.find(btn);
        if (mapIt != settings.mappings.end() && mapIt->second.type == InputType::GamepadAxis)
            fill = controller.getAxisValue(mapIt->second.code) / 32767.f;

        if (fill > 0.f)
            drawSpritePartialFill(prefix + "-pressed", cx, cy - 8.f, BTN_SCALE, fill, fromLeft);
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

            drawSpriteCentered(filled ? "dpad-gate-filled" : "dpad-gate", cx, cy, WIDGET_SCALE);
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
    auto drawStick = [&](const std::string &layoutKey,
                         const std::string &baseKey,
                         const std::string &axisXBtn, const std::string &axisYBtn,
                         const std::string &l3Btn) {
        auto it = settings.layout.find(layoutKey);
        if (it == settings.layout.end())
            return;

        float cx = it->second.x;
        float cy = it->second.y + offy;

        float dx = 0.f, dy = 0.f;
        auto itX = settings.mappings.find(axisXBtn);
        if (itX != settings.mappings.end() && itX->second.type == InputType::GamepadAxis)
            dx = controller.getAxisValue(itX->second.code) / 32768.f;
        auto itY = settings.mappings.find(axisYBtn);
        if (itY != settings.mappings.end() && itY->second.type == InputType::GamepadAxis)
            dy = controller.getAxisValue(itY->second.code) / 32768.f;

        std::string gateKey = baseKey + "-gate" + (filled ? "-filled" : "");
        drawSpriteCentered(gateKey, cx, cy, WIDGET_SCALE);

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
