#include "settings.h"
#include "test_env_compat.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <cstdlib>

namespace fs = std::filesystem;

class LayoutTest : public ::testing::Test
{
protected:
    fs::path tempDir;
    std::string origXdg;
    bool hadXdg = false;

    void SetUp() override
    {
        tempDir = fs::temp_directory_path() / "layout-test";
        fs::create_directories(tempDir / "faidworks-input-viewer");

        const char *xdg = std::getenv("XDG_CONFIG_HOME");
        if (xdg)
        {
            hadXdg = true;
            origXdg = xdg;
        }
        setenv("XDG_CONFIG_HOME", tempDir.string().c_str(), 1);
    }

    void TearDown() override
    {
        fs::remove_all(tempDir);
        if (hadXdg)
            setenv("XDG_CONFIG_HOME", origXdg.c_str(), 1);
        else
            unsetenv("XDG_CONFIG_HOME");
    }

    void writeLayoutFile(const std::string &content)
    {
        fs::create_directories(tempDir / "faidworks-input-viewer");
        std::ofstream f(tempDir / "faidworks-input-viewer" / "layout.txt");
        f << content;
    }
};

TEST_F(LayoutTest, DefaultLayoutLoadsWhenNoFile)
{
    Settings settings;
    settings.loadLayout();

    EXPECT_EQ(settings.layout.size(), 13u);
    EXPECT_FLOAT_EQ(settings.layout["A"].x, 180.f);
    EXPECT_FLOAT_EQ(settings.layout["A"].y, 72.f);
    EXPECT_FLOAT_EQ(settings.layout["DPad"].x, 150.f);
    EXPECT_FLOAT_EQ(settings.layout["DPad"].y, 340.f);
}

TEST_F(LayoutTest, LoadOverridesDefaults)
{
    writeLayoutFile("A=50.5,100.25\nB=200,300\n");

    Settings settings;
    settings.loadLayout();

    EXPECT_FLOAT_EQ(settings.layout["A"].x, 50.5f);
    EXPECT_FLOAT_EQ(settings.layout["A"].y, 100.25f);
    EXPECT_FLOAT_EQ(settings.layout["B"].x, 200.f);
    EXPECT_FLOAT_EQ(settings.layout["B"].y, 300.f);
    // Unmentioned elements keep defaults
    EXPECT_FLOAT_EQ(settings.layout["DPad"].x, 150.f);
}

TEST_F(LayoutTest, SaveAndReload)
{
    Settings settings;
    settings.loadLayout();
    settings.layout["A"] = {99.f, 88.f};
    settings.layout["RStick"] = {500.f, 400.f};
    settings.saveLayout();

    Settings loaded;
    loaded.loadLayout();
    EXPECT_FLOAT_EQ(loaded.layout["A"].x, 99.f);
    EXPECT_FLOAT_EQ(loaded.layout["A"].y, 88.f);
    EXPECT_FLOAT_EQ(loaded.layout["RStick"].x, 500.f);
    EXPECT_FLOAT_EQ(loaded.layout["RStick"].y, 400.f);
}

TEST_F(LayoutTest, MalformedLinesSkipped)
{
    writeLayoutFile("A=50,100\ngarbage\nB=nocomma\nX=abc,def\nY=200,300\n");

    Settings settings;
    settings.loadLayout();

    EXPECT_FLOAT_EQ(settings.layout["A"].x, 50.f);
    EXPECT_FLOAT_EQ(settings.layout["A"].y, 100.f);
    EXPECT_FLOAT_EQ(settings.layout["Y"].x, 200.f);
    EXPECT_FLOAT_EQ(settings.layout["Y"].y, 300.f);
    // B and X should keep defaults due to parse failure
    EXPECT_FLOAT_EQ(settings.layout["B"].x, 290.f);
    EXPECT_FLOAT_EQ(settings.layout["X"].x, 400.f);
}

TEST_F(LayoutTest, EmptyFileKeepsDefaults)
{
    writeLayoutFile("");

    Settings settings;
    settings.loadLayout();

    EXPECT_EQ(settings.layout.size(), 13u);
    EXPECT_FLOAT_EQ(settings.layout["Start"].x, 620.f);
}
