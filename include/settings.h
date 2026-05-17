#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>
#include <vector>

enum class InputType { None, Key, GamepadButton, GamepadAxis };
enum class ActiveStyle { Filled, Pressed };

struct InputMapping {
    InputType type = InputType::None;
    int code = -1;
    std::string name;
};

inline const std::vector<std::string> GAMEPAD_BUTTONS = {
    "A", "B", "X", "Y",
    "LB", "RB", "LT", "RT",
    "Start", "Select", "L3", "R3",
    "DPad Up", "DPad Down", "DPad Left", "DPad Right",
    "LStick X", "LStick Y", "RStick X", "RStick Y"
};

struct ElementLayout {
    float x, y;
};

inline const std::vector<std::string> LAYOUT_ELEMENTS = {
    "A", "B", "X", "Y", "Start",
    "LB", "RB", "LT", "RT", "Select",
    "DPad", "LStick", "RStick"
};

inline std::string elementForButton(const std::string &btn)
{
    if (btn == "DPad Up" || btn == "DPad Down" || btn == "DPad Left" || btn == "DPad Right")
        return "DPad";
    if (btn == "LStick X" || btn == "LStick Y" || btn == "L3")
        return "LStick";
    if (btn == "RStick X" || btn == "RStick Y" || btn == "R3")
        return "RStick";
    return btn;
}

class Settings
{
public:
    void load();
    void save();
    void loadLayout();
    void saveLayout();

    std::map<std::string, InputMapping> mappings;
    ActiveStyle activeStyle = ActiveStyle::Filled;
    std::string bgColor = "000000";
    std::string fontPath = "resources/fonts/OpenDyslexic-Regular.otf";
    int fpsLimit = 60;
    int deadzone = 5;
    int inputGroupFrames = 0;
    int historyKey = 11; // sf::Keyboard::Scancode::H
    std::string historyKeyName = "h";
    bool trackFrames = false;
    std::map<std::string, ElementLayout> layout;

    std::map<std::string, std::string> elementInactiveColors;
    std::map<std::string, std::string> elementActiveColors;

    std::string getInactiveColor(const std::string &element) const;
    std::string getActiveColor(const std::string &element) const;
};

#endif // SETTINGS_H
