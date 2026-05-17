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
        fs::create_directories(tempDir / "input-viewer");

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
        fs::create_directories(tempDir / "input-viewer");
        std::ofstream f(tempDir / "input-viewer" / "settings.txt");
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
