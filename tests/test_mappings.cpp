#include "settings.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <cstdlib>

namespace fs = std::filesystem;

class MappingsTest : public ::testing::Test
{
protected:
    fs::path tempDir;
    std::string origXdg;
    bool hadXdg = false;

    void SetUp() override
    {
        tempDir = fs::temp_directory_path() / "mappings-test";
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

TEST_F(MappingsTest, LoadButtonMapping)
{
    writeSettingsFile("A=button:0:Cross\nB=button:1:Circle\n");

    Settings settings;
    settings.load();

    ASSERT_TRUE(settings.mappings.count("A"));
    EXPECT_EQ(settings.mappings["A"].type, InputType::GamepadButton);
    EXPECT_EQ(settings.mappings["A"].code, 0);
    EXPECT_EQ(settings.mappings["A"].name, "Cross");

    ASSERT_TRUE(settings.mappings.count("B"));
    EXPECT_EQ(settings.mappings["B"].type, InputType::GamepadButton);
    EXPECT_EQ(settings.mappings["B"].code, 1);
    EXPECT_EQ(settings.mappings["B"].name, "Circle");
}

TEST_F(MappingsTest, LoadKeyMapping)
{
    writeSettingsFile("Start=key:13:Enter\n");

    Settings settings;
    settings.load();

    ASSERT_TRUE(settings.mappings.count("Start"));
    EXPECT_EQ(settings.mappings["Start"].type, InputType::Key);
    EXPECT_EQ(settings.mappings["Start"].code, 13);
    EXPECT_EQ(settings.mappings["Start"].name, "Enter");
}

TEST_F(MappingsTest, LoadAxisMapping)
{
    writeSettingsFile("LStick X=axis:0:Left X\n");

    Settings settings;
    settings.load();

    ASSERT_TRUE(settings.mappings.count("LStick X"));
    EXPECT_EQ(settings.mappings["LStick X"].type, InputType::GamepadAxis);
    EXPECT_EQ(settings.mappings["LStick X"].code, 0);
    EXPECT_EQ(settings.mappings["LStick X"].name, "Left X");
}

TEST_F(MappingsTest, InvalidTypesSkipped)
{
    writeSettingsFile("A=invalid:0:Foo\nB=button:1:Bar\n");

    Settings settings;
    settings.load();

    EXPECT_FALSE(settings.mappings.count("A"));
    ASSERT_TRUE(settings.mappings.count("B"));
}

TEST_F(MappingsTest, InvalidCodeSkipped)
{
    writeSettingsFile("A=button:notanumber:Foo\nB=button:2:Bar\n");

    Settings settings;
    settings.load();

    EXPECT_FALSE(settings.mappings.count("A"));
    ASSERT_TRUE(settings.mappings.count("B"));
}

TEST_F(MappingsTest, MissingColonsSkipped)
{
    writeSettingsFile("A=button0Foo\nB=button:1:Bar\n");

    Settings settings;
    settings.load();

    EXPECT_FALSE(settings.mappings.count("A"));
    ASSERT_TRUE(settings.mappings.count("B"));
}

TEST_F(MappingsTest, SaveAndReloadMappings)
{
    Settings settings;
    settings.mappings["X"] = {InputType::GamepadButton, 3, "Square"};
    settings.mappings["LStick Y"] = {InputType::GamepadAxis, 1, "Left Y"};
    settings.save();

    Settings loaded;
    loaded.load();

    ASSERT_TRUE(loaded.mappings.count("X"));
    EXPECT_EQ(loaded.mappings["X"].type, InputType::GamepadButton);
    EXPECT_EQ(loaded.mappings["X"].code, 3);
    EXPECT_EQ(loaded.mappings["X"].name, "Square");

    ASSERT_TRUE(loaded.mappings.count("LStick Y"));
    EXPECT_EQ(loaded.mappings["LStick Y"].type, InputType::GamepadAxis);
    EXPECT_EQ(loaded.mappings["LStick Y"].code, 1);
    EXPECT_EQ(loaded.mappings["LStick Y"].name, "Left Y");
}

TEST_F(MappingsTest, NoneMappingsNotSaved)
{
    Settings settings;
    settings.mappings["A"] = {InputType::None, -1, ""};
    settings.mappings["B"] = {InputType::GamepadButton, 1, "Circle"};
    settings.save();

    Settings loaded;
    loaded.load();

    EXPECT_FALSE(loaded.mappings.count("A"));
    ASSERT_TRUE(loaded.mappings.count("B"));
}

TEST_F(MappingsTest, MappingsAndMetadataCoexist)
{
    writeSettingsFile(
        "_bgColor=112233\n"
        "_fontPath=/some/font.ttf\n"
        "A=button:0:Cross\n"
        "LT=axis:4:L2\n");

    Settings settings;
    settings.load();

    EXPECT_EQ(settings.bgColor, "112233");
    EXPECT_EQ(settings.fontPath, "/some/font.ttf");
    ASSERT_TRUE(settings.mappings.count("A"));
    EXPECT_EQ(settings.mappings["A"].code, 0);
    ASSERT_TRUE(settings.mappings.count("LT"));
    EXPECT_EQ(settings.mappings["LT"].type, InputType::GamepadAxis);
}
