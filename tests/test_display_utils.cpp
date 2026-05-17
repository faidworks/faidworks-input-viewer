#include "display.h"
#include <gtest/gtest.h>

TEST(ParseHexColor, ValidColors)
{
    sf::Color c = parseHexColor("FF0000");
    EXPECT_EQ(c.r, 255);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);

    c = parseHexColor("00FF00");
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 255);
    EXPECT_EQ(c.b, 0);

    c = parseHexColor("0000FF");
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 255);

    c = parseHexColor("FFFFFF");
    EXPECT_EQ(c.r, 255);
    EXPECT_EQ(c.g, 255);
    EXPECT_EQ(c.b, 255);

    c = parseHexColor("000000");
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
}

TEST(ParseHexColor, MixedCase)
{
    sf::Color c = parseHexColor("aaBBcc");
    EXPECT_EQ(c.r, 0xAA);
    EXPECT_EQ(c.g, 0xBB);
    EXPECT_EQ(c.b, 0xCC);
}

TEST(ParseHexColor, InvalidLength)
{
    sf::Color c = parseHexColor("FFF");
    EXPECT_EQ(c, sf::Color::Black);

    c = parseHexColor("FFFFFFF");
    EXPECT_EQ(c, sf::Color::Black);

    c = parseHexColor("");
    EXPECT_EQ(c, sf::Color::Black);
}

TEST(ParseHexColor, InvalidCharacters)
{
    sf::Color c = parseHexColor("ZZZZZZ");
    EXPECT_EQ(c, sf::Color::Black);
}
