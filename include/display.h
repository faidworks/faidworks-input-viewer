#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <utility>
#include "controller.h"
#include "settings.h"

struct HistoryEntry {
    std::vector<std::string> buttons;
    int frameGap = 0;
};

enum class ViewMode { Main, Layout, Settings };

class Display
{
public:
    Display();
    ~Display();
    void processEvents(Settings &settings, Controller &controller);
    void render(const Settings &settings, const Controller &controller);
    void renderLayout(Settings &settings);
    void renderSettings(const Settings &settings);
    bool isOpen() const;
    ViewMode getViewMode() const;
    void loadFont(const std::string &path);
    void setFramerateLimit(int limit);
    void reloadTextures(const Settings &settings);
    void updateHistory(const Settings &settings, const Controller &controller);
    void renderHistory(const Settings &settings);
    void toggleHistoryWindow();

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;
    ViewMode viewMode = ViewMode::Main;
    bool navBarVisible = false;
    int detectingIndex = -1;
    float settingsScroll = 0.f;

    std::map<std::string, sf::Texture> textures;
    sf::Shader tintShader;
    bool shaderLoaded = false;
    void loadTextures(const Settings &settings);
    void scanSystemFonts();
    void drawSpriteCentered(const std::string &key, float cx, float cy, float scale);
    void drawSpriteTinted(const std::string &key, float cx, float cy, float scale, sf::Color tint);
    void drawSpritePartialFill(const std::string &key, float cx, float cy, float scale, float fill, bool fromLeft, sf::Color tint = sf::Color::White);
    void drawNavBar();
    float navBarHeight() const;

    std::vector<std::pair<std::string, std::string>> availableFonts;
    int fontIndex = 0;
    bool fontPickerOpen = false;
    float fontPickerScroll = 0.f;
    std::map<int, sf::Font> fontPreviewCache;

    bool presetPickerOpen = false;
    float presetPickerScroll = 0.f;
    std::vector<std::string> presetList;
    int selectedPresetIndex = -1;

    bool creatingPreset = false;
    std::string newPresetName;

    int confirmDeleteIndex = -1;

    bool editingImageFolder = false;
    std::string imageFolderInput;

    std::string draggingElement;
    sf::Vector2f dragOffset;

    bool editingBgColor = false;
    std::string bgColorInput;

    int editingColorIndex = -1;
    bool editingColorIsActive = false;
    std::string colorInput;

    bool draggingDeadzone = false;
    bool detectingHistoryKey = false;

    bool draggingGroupFrames = false;

    sf::RenderWindow historyWindow;
    bool historyOpen = false;
    std::deque<HistoryEntry> inputHistory;
    std::set<std::string> prevActiveButtons;
    int framesSinceLastEntry = 0;
};

sf::Color parseHexColor(const std::string &hex);

#endif // DISPLAY_H
