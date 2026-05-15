#include "controller.h"
#include "settings.h"
#include "display.h"

int main()
{
    Settings settings;
    Display display;
    Controller controller;

    settings.load();
    settings.loadLayout();

    while (display.isOpen())
    {
        controller.update();
        display.processEvents(settings, controller);

        if (!display.isOpen())
            break;

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
    }

    settings.save();
    settings.saveLayout();
    return 0;
}
