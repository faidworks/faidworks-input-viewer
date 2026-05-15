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
    std::map<std::string, ElementLayout> layout;
};

#endif // SETTINGS_H
