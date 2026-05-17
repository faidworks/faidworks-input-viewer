#include "settings.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

static fs::path configDir()
{
#ifdef _WIN32
    const char *appdata = std::getenv("APPDATA");
    fs::path dir = appdata ? fs::path(std::string(appdata)) / "FaidworksInputViewer" : fs::current_path();
#else
    const char *xdg = std::getenv("XDG_CONFIG_HOME");
    fs::path dir;
    if (xdg)
        dir = fs::path(std::string(xdg)) / "faidworks-input-viewer";
    else
    {
        const char *home = std::getenv("HOME");
        dir = home ? fs::path(std::string(home)) / ".config" / "faidworks-input-viewer" : fs::current_path();
    }
#endif
    fs::create_directories(dir);
    return dir;
}

static fs::path configPath(const std::string &filename)
{
    return configDir() / filename;
}

void Settings::load()
{
    std::ifstream file(configPath("settings.txt"));
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

        if (button == "_fontPath")
        {
            fontPath = value;
            continue;
        }

        if (button == "_fpsLimit")
        {
            try { fpsLimit = std::stoi(value); } catch (...) {}
            continue;
        }

        if (button == "_deadzone")
        {
            try { deadzone = std::stoi(value); } catch (...) {}
            continue;
        }

        if (button == "_inputGroupFrames")
        {
            try { inputGroupFrames = std::stoi(value); } catch (...) {}
            continue;
        }

        if (button == "_trackFrames")
        {
            trackFrames = (value == "true");
            continue;
        }

        if (button == "_historyKey")
        {
            auto colon = value.find(':');
            if (colon != std::string::npos)
            {
                try { historyKey = std::stoi(value.substr(0, colon)); } catch (...) {}
                historyKeyName = value.substr(colon + 1);
            }
            continue;
        }

        if (button.rfind("_colorInactive_", 0) == 0)
        {
            std::string elem = button.substr(15);
            elementInactiveColors[elem] = value;
            continue;
        }

        if (button.rfind("_colorActive_", 0) == 0)
        {
            std::string elem = button.substr(13);
            elementActiveColors[elem] = value;
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
        catch (const std::invalid_argument &)
        {
            continue;
        }
        catch (const std::out_of_range &)
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

std::string Settings::getInactiveColor(const std::string &element) const
{
    auto it = elementInactiveColors.find(element);
    return (it != elementInactiveColors.end()) ? it->second : "FFFFFF";
}

std::string Settings::getActiveColor(const std::string &element) const
{
    auto it = elementActiveColors.find(element);
    return (it != elementActiveColors.end()) ? it->second : "FFFFFF";
}

void Settings::save()
{
    std::ofstream file(configPath("settings.txt"));
    if (!file.is_open())
        return;

    file << "_activeStyle=" << (activeStyle == ActiveStyle::Pressed ? "pressed" : "filled") << "\n";
    file << "_bgColor=" << bgColor << "\n";
    file << "_fontPath=" << fontPath << "\n";
    file << "_fpsLimit=" << fpsLimit << "\n";
    file << "_deadzone=" << deadzone << "\n";
    file << "_inputGroupFrames=" << inputGroupFrames << "\n";
    file << "_trackFrames=" << (trackFrames ? "true" : "false") << "\n";
    file << "_historyKey=" << historyKey << ":" << historyKeyName << "\n";

    for (const auto &[elem, color] : elementInactiveColors)
        file << "_colorInactive_" << elem << "=" << color << "\n";
    for (const auto &[elem, color] : elementActiveColors)
        file << "_colorActive_" << elem << "=" << color << "\n";

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

    std::ifstream file(configPath("layout.txt"));
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
        catch (const std::invalid_argument &)
        {
            continue;
        }
        catch (const std::out_of_range &)
        {
            continue;
        }
    }
}

void Settings::saveLayout()
{
    std::ofstream file(configPath("layout.txt"));
    if (!file.is_open())
        return;

    for (const auto &[name, pos] : layout)
        file << name << "=" << pos.x << "," << pos.y << "\n";
}
