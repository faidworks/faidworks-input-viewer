#include <SFML/Graphics.hpp>
#include "display.h"
#include "display_constants.h"

void Display::renderLayout(Settings &settings)
{
    window.clear(sf::Color(20, 20, 30));
    drawNavBar();
    float offy = navBarHeight();


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
        text.setPosition({cx - b.size.x / 2.f, cy - b.size.y / 2.f});
        window.draw(text);
    }

    window.display();
}
