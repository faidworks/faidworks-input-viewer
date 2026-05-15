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
