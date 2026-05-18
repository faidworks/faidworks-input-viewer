#include "settings.h"
#include <algorithm>
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

fs::path Settings::presetsDir()
{
    fs::path dir = configDir() / "presets";
    fs::create_directories(dir);
    return dir;
}

fs::path Settings::presetDir(const std::string &name)
{
    fs::path dir = presetsDir() / name;
    fs::create_directories(dir);
    return dir;
}

std::vector<std::string> Settings::listPresets() const
{
    std::vector<std::string> result;
    result.push_back("Default");

    try
    {
        for (const auto &entry : fs::directory_iterator(presetsDir()))
        {
            if (!entry.is_directory())
                continue;
            std::string name = entry.path().filename().string();
            if (name != "Default")
                result.push_back(name);
        }
    }
    catch (...) {}

    std::sort(result.begin() + 1, result.end());
    return result;
}

static void migrateOldDefault()
{
    fs::path oldDefault = Settings::presetsDir() / "Default";
    if (fs::exists(oldDefault) && fs::is_directory(oldDefault))
    {
        fs::path newDefault = Settings::presetsDir() / "Default Filled";
        if (!fs::exists(newDefault))
        {
            try { fs::rename(oldDefault, newDefault); } catch (...) {}
        }
    }
}

static void createDefaultPreset(const std::string &name, const std::string &builtinActive)
{
    fs::path dir = Settings::presetDir(name);
    fs::path presetFile = dir / "preset.txt";
    if (fs::exists(presetFile))
        return;

    fs::path rootLayout = configDir() / "layout.txt";
    fs::path presetLayout = dir / "layout.txt";
    if (fs::exists(rootLayout) && !fs::exists(presetLayout))
    {
        try { fs::copy_file(rootLayout, presetLayout); } catch (...) {}
    }

    std::ofstream pf(presetFile);
    if (pf.is_open())
    {
        pf << "_imageFolder=\n";
        pf << "_builtinActive=" << builtinActive << "\n";
    }
}

void Settings::loadPreset(const std::string &name)
{
    activePreset = name;
    presetConfig = PresetConfig{};

    migrateOldDefault();
    createDefaultPreset("Default Filled", "filled");
    createDefaultPreset("Default Pressed", "pressed");

    fs::path dir = presetDir(name);
    fs::path presetFile = dir / "preset.txt";

    {
        std::ifstream file(presetFile);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                auto eq = line.find('=');
                if (eq == std::string::npos)
                    continue;
                std::string key = line.substr(0, eq);
                std::string val = line.substr(eq + 1);

                if (key == "_imageFolder")
                    presetConfig.imageFolder = val;
                else if (key == "_builtinActive")
                    presetConfig.builtinActive = val;
                else if (key.rfind("_override_", 0) == 0)
                    presetConfig.imageOverrides[key.substr(10)] = val;
            }
        }
    }

    loadLayout();
}

void Settings::savePreset(const std::string &name)
{
    fs::path dir = presetDir(name);

    {
        std::ofstream file(dir / "preset.txt");
        if (file.is_open())
        {
            file << "_imageFolder=" << presetConfig.imageFolder << "\n";
            file << "_builtinActive=" << presetConfig.builtinActive << "\n";
            for (const auto &[key, path] : presetConfig.imageOverrides)
                file << "_override_" << key << "=" << path << "\n";
        }
    }

    saveLayout();
}

void Settings::deletePreset(const std::string &name)
{
    if (name == "Default")
        return;
    try { fs::remove_all(presetsDir() / name); } catch (...) {}
}

void Settings::renamePreset(const std::string &oldName, const std::string &newName)
{
    if (oldName == "Default" || newName.empty())
        return;
    try
    {
        fs::rename(presetsDir() / oldName, presetsDir() / newName);
        if (activePreset == oldName)
            activePreset = newName;
    }
    catch (...) {}
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

        if (button == "_activePreset")
        {
            activePreset = value;
            continue;
        }

        if (button == "_activeStyle")
        {
            if (value == "pressed" && activePreset == "Default")
                activePreset = "Default Pressed";
            else if (activePreset == "Default")
                activePreset = "Default Filled";
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

        if (button == "_trackSticks")
        {
            trackSticks = (value == "true");
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

    file << "_activePreset=" << activePreset << "\n";
    file << "_bgColor=" << bgColor << "\n";
    file << "_fontPath=" << fontPath << "\n";
    file << "_fpsLimit=" << fpsLimit << "\n";
    file << "_deadzone=" << deadzone << "\n";
    file << "_inputGroupFrames=" << inputGroupFrames << "\n";
    file << "_trackFrames=" << (trackFrames ? "true" : "false") << "\n";
    file << "_trackSticks=" << (trackSticks ? "true" : "false") << "\n";
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

    fs::path layoutPath = presetDir(activePreset) / "layout.txt";
    if (!fs::exists(layoutPath))
        layoutPath = configPath("layout.txt");
    std::ifstream file(layoutPath);
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
    std::ofstream file(presetDir(activePreset) / "layout.txt");
    if (!file.is_open())
        return;

    for (const auto &[name, pos] : layout)
        file << name << "=" << pos.x << "," << pos.y << "\n";
}
