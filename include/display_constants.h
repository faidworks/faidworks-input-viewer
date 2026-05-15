#ifndef DISPLAY_CONSTANTS_H
#define DISPLAY_CONSTANTS_H

static const float ROW_HEIGHT = 32.f;
static const float ROW_START_Y = 100.f;
static const float ROW_X = 50.f;
static const float ROW_WIDTH = 700.f;
static const float NAV_HEIGHT = 30.f;
static const float BTN_SCALE = 0.5f;
static const float WIDGET_SCALE = 0.85f;

#include <string>
#include <cctype>

inline std::string toLower(const std::string &s)
{
    std::string r = s;
    for (char &c : r)
        c = std::tolower(c);
    return r;
}

#endif // DISPLAY_CONSTANTS_H
