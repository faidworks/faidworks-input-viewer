#include "settings.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

class FontSettingsTest : public ::testing::Test
{
protected:
    fs::path tempDir;
    std::string origXdg;
    bool hadXdg = false;

    void SetUp() override
    {
        tempDir = fs::temp_directory_path() / "input-viewer-test";
        fs::create_directories(tempDir / "faidworks-input-viewer");

        const char *xdg = std::getenv("XDG_CONFIG_HOME");
        if (xdg)
        {
            hadXdg = true;
            origXdg = xdg;
        }
        setenv("XDG_CONFIG_HOME", tempDir.c_str(), 1);
    }

    void TearDown() override
    {
        fs::remove_all(tempDir);
        if (hadXdg)
            setenv("XDG_CONFIG_HOME", origXdg.c_str(), 1);
        else
            unsetenv("XDG_CONFIG_HOME");
    }

    void writeSettingsFile(const std::string &content)
    {
        fs::create_directories(tempDir / "faidworks-input-viewer");
        std::ofstream f(tempDir / "faidworks-input-viewer" / "settings.txt");
        f << content;
    }
};

TEST_F(FontSettingsTest, DefaultFontPath)
{
    Settings settings;
    EXPECT_EQ(settings.fontPath, "resources/fonts/OpenDyslexic-Regular.otf");
}

TEST_F(FontSettingsTest, LoadFontPathFromFile)
{
    writeSettingsFile("_fontPath=/usr/share/fonts/TTF/DejaVuSans.ttf\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.fontPath, "/usr/share/fonts/TTF/DejaVuSans.ttf");
}

TEST_F(FontSettingsTest, LoadPreservesDefaultWhenMissing)
{
    writeSettingsFile("_bgColor=112233\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.fontPath, "resources/fonts/OpenDyslexic-Regular.otf");
    EXPECT_EQ(settings.bgColor, "112233");
}

TEST_F(FontSettingsTest, SaveAndReloadFontPath)
{
    Settings settings;
    settings.fontPath = "/home/user/.fonts/MyFont.otf";
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.fontPath, "/home/user/.fonts/MyFont.otf");
}

TEST_F(FontSettingsTest, SavePreservesOtherSettings)
{
    Settings settings;
    settings.fontPath = "/some/font.ttf";
    settings.bgColor = "AABBCC";
    settings.activeStyle = ActiveStyle::Pressed;
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.fontPath, "/some/font.ttf");
    EXPECT_EQ(loaded.bgColor, "AABBCC");
    EXPECT_EQ(loaded.activeStyle, ActiveStyle::Pressed);
}

TEST_F(FontSettingsTest, FontPathWithSpaces)
{
    writeSettingsFile("_fontPath=/usr/share/fonts/Some Font Name.ttf\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.fontPath, "/usr/share/fonts/Some Font Name.ttf");
}

TEST_F(FontSettingsTest, DefaultFpsLimit)
{
    Settings settings;
    EXPECT_EQ(settings.fpsLimit, 60);
}

TEST_F(FontSettingsTest, LoadFpsLimit)
{
    writeSettingsFile("_fpsLimit=144\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.fpsLimit, 144);
}

TEST_F(FontSettingsTest, LoadFpsLimitUnlimited)
{
    writeSettingsFile("_fpsLimit=0\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.fpsLimit, 0);
}

TEST_F(FontSettingsTest, SaveAndReloadFpsLimit)
{
    Settings settings;
    settings.fpsLimit = 120;
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.fpsLimit, 120);
}

TEST_F(FontSettingsTest, DefaultDeadzone)
{
    Settings settings;
    EXPECT_EQ(settings.deadzone, 5);
}

TEST_F(FontSettingsTest, LoadDeadzone)
{
    writeSettingsFile("_deadzone=25\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.deadzone, 25);
}

TEST_F(FontSettingsTest, SaveAndReloadDeadzone)
{
    Settings settings;
    settings.deadzone = 50;
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.deadzone, 50);
}

TEST_F(FontSettingsTest, DeadzoneZero)
{
    writeSettingsFile("_deadzone=0\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.deadzone, 0);
}

TEST_F(FontSettingsTest, DefaultInputGroupFrames)
{
    Settings settings;
    EXPECT_EQ(settings.inputGroupFrames, 0);
}

TEST_F(FontSettingsTest, LoadInputGroupFrames)
{
    writeSettingsFile("_inputGroupFrames=3\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.inputGroupFrames, 3);
}

TEST_F(FontSettingsTest, SaveAndReloadInputGroupFrames)
{
    Settings settings;
    settings.inputGroupFrames = 5;
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.inputGroupFrames, 5);
}

TEST_F(FontSettingsTest, DefaultHistoryKey)
{
    Settings settings;
    EXPECT_EQ(settings.historyKey, 11);
    EXPECT_EQ(settings.historyKeyName, "h");
}

TEST_F(FontSettingsTest, LoadHistoryKey)
{
    writeSettingsFile("_historyKey=58:Space\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.historyKey, 58);
    EXPECT_EQ(settings.historyKeyName, "Space");
}

TEST_F(FontSettingsTest, SaveAndReloadHistoryKey)
{
    Settings settings;
    settings.historyKey = 22;
    settings.historyKeyName = "F1";
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.historyKey, 22);
    EXPECT_EQ(loaded.historyKeyName, "F1");
}

TEST_F(FontSettingsTest, HistoryKeyPreservesDefaultWhenMissing)
{
    writeSettingsFile("_bgColor=112233\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.historyKey, 11);
    EXPECT_EQ(settings.historyKeyName, "h");
}

TEST_F(FontSettingsTest, AllNewSettingsCoexist)
{
    writeSettingsFile(
        "_fpsLimit=170\n"
        "_deadzone=15\n"
        "_inputGroupFrames=2\n"
        "_historyKey=7:Enter\n"
        "_bgColor=AABBCC\n"
        "_fontPath=/some/font.ttf\n");

    Settings settings;
    settings.load();
    EXPECT_EQ(settings.fpsLimit, 170);
    EXPECT_EQ(settings.deadzone, 15);
    EXPECT_EQ(settings.inputGroupFrames, 2);
    EXPECT_EQ(settings.historyKey, 7);
    EXPECT_EQ(settings.historyKeyName, "Enter");
    EXPECT_EQ(settings.bgColor, "AABBCC");
    EXPECT_EQ(settings.fontPath, "/some/font.ttf");
}

TEST_F(FontSettingsTest, SavePreservesAllSettings)
{
    Settings settings;
    settings.fpsLimit = 30;
    settings.deadzone = 10;
    settings.inputGroupFrames = 4;
    settings.historyKey = 15;
    settings.historyKeyName = "Tab";
    settings.fontPath = "/test/font.otf";
    settings.bgColor = "112233";
    settings.activeStyle = ActiveStyle::Pressed;
    settings.save();

    Settings loaded;
    loaded.load();
    EXPECT_EQ(loaded.fpsLimit, 30);
    EXPECT_EQ(loaded.deadzone, 10);
    EXPECT_EQ(loaded.inputGroupFrames, 4);
    EXPECT_EQ(loaded.historyKey, 15);
    EXPECT_EQ(loaded.historyKeyName, "Tab");
    EXPECT_EQ(loaded.fontPath, "/test/font.otf");
    EXPECT_EQ(loaded.bgColor, "112233");
    EXPECT_EQ(loaded.activeStyle, ActiveStyle::Pressed);
}
