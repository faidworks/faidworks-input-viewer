#include "settings.h"
#include <gtest/gtest.h>

TEST(ElementForButton, DirectButtons)
{
    EXPECT_EQ(elementForButton("A"), "A");
    EXPECT_EQ(elementForButton("B"), "B");
    EXPECT_EQ(elementForButton("X"), "X");
    EXPECT_EQ(elementForButton("Y"), "Y");
    EXPECT_EQ(elementForButton("LB"), "LB");
    EXPECT_EQ(elementForButton("RB"), "RB");
    EXPECT_EQ(elementForButton("LT"), "LT");
    EXPECT_EQ(elementForButton("RT"), "RT");
    EXPECT_EQ(elementForButton("Start"), "Start");
    EXPECT_EQ(elementForButton("Select"), "Select");
}

TEST(ElementForButton, DPadDirections)
{
    EXPECT_EQ(elementForButton("DPad Up"), "DPad");
    EXPECT_EQ(elementForButton("DPad Down"), "DPad");
    EXPECT_EQ(elementForButton("DPad Left"), "DPad");
    EXPECT_EQ(elementForButton("DPad Right"), "DPad");
}

TEST(ElementForButton, StickAxes)
{
    EXPECT_EQ(elementForButton("LStick X"), "LStick");
    EXPECT_EQ(elementForButton("LStick Y"), "LStick");
    EXPECT_EQ(elementForButton("RStick X"), "RStick");
    EXPECT_EQ(elementForButton("RStick Y"), "RStick");
}

TEST(ElementForButton, StickButtons)
{
    EXPECT_EQ(elementForButton("L3"), "LStick");
    EXPECT_EQ(elementForButton("R3"), "RStick");
}

class SettingsColorTest : public ::testing::Test
{
protected:
    Settings settings;
};

TEST_F(SettingsColorTest, DefaultColorIsWhite)
{
    EXPECT_EQ(settings.getInactiveColor("A"), "FFFFFF");
    EXPECT_EQ(settings.getActiveColor("A"), "FFFFFF");
    EXPECT_EQ(settings.getInactiveColor("DPad"), "FFFFFF");
    EXPECT_EQ(settings.getActiveColor("LStick"), "FFFFFF");
}

TEST_F(SettingsColorTest, SetAndGetInactiveColor)
{
    settings.elementInactiveColors["A"] = "FF0000";
    EXPECT_EQ(settings.getInactiveColor("A"), "FF0000");
    EXPECT_EQ(settings.getActiveColor("A"), "FFFFFF");
}

TEST_F(SettingsColorTest, SetAndGetActiveColor)
{
    settings.elementActiveColors["B"] = "00FF00";
    EXPECT_EQ(settings.getActiveColor("B"), "00FF00");
    EXPECT_EQ(settings.getInactiveColor("B"), "FFFFFF");
}

TEST_F(SettingsColorTest, DifferentElementsIndependent)
{
    settings.elementInactiveColors["A"] = "FF0000";
    settings.elementInactiveColors["B"] = "00FF00";
    settings.elementActiveColors["A"] = "0000FF";

    EXPECT_EQ(settings.getInactiveColor("A"), "FF0000");
    EXPECT_EQ(settings.getInactiveColor("B"), "00FF00");
    EXPECT_EQ(settings.getActiveColor("A"), "0000FF");
    EXPECT_EQ(settings.getActiveColor("B"), "FFFFFF");
}

TEST_F(SettingsColorTest, SharedElementColor)
{
    settings.elementInactiveColors["DPad"] = "AABBCC";
    EXPECT_EQ(settings.getInactiveColor("DPad"), "AABBCC");

    std::string elem = elementForButton("DPad Up");
    EXPECT_EQ(settings.getInactiveColor(elem), "AABBCC");
}
