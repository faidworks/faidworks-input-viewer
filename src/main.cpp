#include "controller.h"
#include "settings.h"
#include "display.h"

int main()
{
    Settings settings;
    Display display;
    Controller controller;

    settings.load();
    settings.loadPreset(settings.activePreset);
    display.loadFont(settings.fontPath);
    display.reloadTextures(settings);
    display.setFramerateLimit(settings.fpsLimit);

    while (display.isOpen())
    {
        controller.update();
        display.processEvents(settings, controller);

        if (!display.isOpen())
            break;

        display.updateHistory(settings, controller);

        switch (display.getViewMode())
        {
        case ViewMode::Main:
            display.render(settings, controller);
            break;
        case ViewMode::Layout:
            display.renderLayout(settings);
            break;
        case ViewMode::Settings:
            display.renderSettings(settings);
            break;
        }

        display.renderHistory(settings);
    }

    settings.save();
    settings.savePreset(settings.activePreset);
    return 0;
}
