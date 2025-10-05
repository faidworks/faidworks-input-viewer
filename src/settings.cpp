#include "settings.h"
#include <fstream>
#include <iostream>

void Settings::load()
{
    std::ifstream inFile("settings.txt");
    if (inFile.is_open())
    {
        // Load settings from file
        inFile.close();
    }
}

void Settings::save()
{
    std::ofstream outFile("settings.txt");
    if (outFile.is_open())
    {
        // Save settings to file
        outFile.close();
    }
}
