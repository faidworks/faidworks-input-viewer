#include "settings.h"
#include <fstream>
#include <iostream>

void Settings::load()
{
    std::ifstream file("settings.txt");
    if (!file.is_open())
        return;

    std::string line;
    while (std::getline(file, line))
    {
        auto eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string button = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if (button == "_activeStyle")
        {
            activeStyle = (value == "pressed") ? ActiveStyle::Pressed : ActiveStyle::Filled;
            continue;
        }

        if (button == "_bgColor")
        {
            bgColor = value;
            continue;
        }

        auto colon1 = value.find(':');
        auto colon2 = value.find(':', colon1 + 1);
        if (colon1 == std::string::npos || colon2 == std::string::npos)
            continue;

        std::string typeStr = value.substr(0, colon1);
        std::string codeStr = value.substr(colon1 + 1, colon2 - colon1 - 1);
        std::string name = value.substr(colon2 + 1);

        InputMapping mapping;
        try
        {
            mapping.code = std::stoi(codeStr);
        }
        catch (...)
        {
            continue;
        }
        mapping.name = name;

        if (typeStr == "key")
            mapping.type = InputType::Key;
        else if (typeStr == "button")
            mapping.type = InputType::GamepadButton;
        else if (typeStr == "axis")
            mapping.type = InputType::GamepadAxis;
        else
            continue;

        mappings[button] = mapping;
    }
}

void Settings::save()
{
    std::ofstream file("settings.txt");
    if (!file.is_open())
        return;

    file << "_activeStyle=" << (activeStyle == ActiveStyle::Pressed ? "pressed" : "filled") << "\n";
    file << "_bgColor=" << bgColor << "\n";

    for (const auto &[button, mapping] : mappings)
    {
        if (mapping.type == InputType::None)
            continue;

        std::string typeStr;
        switch (mapping.type)
        {
        case InputType::Key:
            typeStr = "key";
            break;
        case InputType::GamepadButton:
            typeStr = "button";
            break;
        case InputType::GamepadAxis:
            typeStr = "axis";
            break;
        default:
            continue;
        }

        file << button << "=" << typeStr << ":" << mapping.code << ":" << mapping.name << "\n";
    }
}

static const std::map<std::string, ElementLayout> DEFAULT_LAYOUT = {
    {"A", {180.f, 72.f}},
    {"B", {290.f, 72.f}},
    {"X", {400.f, 72.f}},
    {"Y", {510.f, 72.f}},
    {"Start", {620.f, 72.f}},
    {"LB", {180.f, 172.f}},
    {"RB", {290.f, 172.f}},
    {"LT", {400.f, 172.f}},
    {"RT", {510.f, 172.f}},
    {"Select", {620.f, 172.f}},
    {"DPad", {150.f, 340.f}},
    {"LStick", {400.f, 340.f}},
    {"RStick", {650.f, 340.f}},
};

void Settings::loadLayout()
{
    layout = DEFAULT_LAYOUT;

    std::ifstream file("layout.txt");
    if (!file.is_open())
        return;

    std::string line;
    while (std::getline(file, line))
    {
        auto eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string name = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        auto comma = value.find(',');
        if (comma == std::string::npos)
            continue;

        try
        {
            float x = std::stof(value.substr(0, comma));
            float y = std::stof(value.substr(comma + 1));
            layout[name] = {x, y};
        }
        catch (...)
        {
            continue;
        }
    }
}

void Settings::saveLayout()
{
    std::ofstream file("layout.txt");
    if (!file.is_open())
        return;

    for (const auto &[name, pos] : layout)
        file << name << "=" << pos.x << "," << pos.y << "\n";
}
