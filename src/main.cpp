#include "controller.h"
#include "settings.h"
#include "display.h"

int main()
{
    Settings settings;
    Display display;
    Controller controller;

    settings.load();

    while (display.isOpen())
    {
        controller.update();
        display.processEvents(settings, controller);

        if (!display.isOpen())
            break;

        if (display.getViewMode() == ViewMode::Main)
        {
            display.render(settings, controller);
        }
        else
        {
            display.renderSettings(settings);
        }
    }

    settings.save();
    return 0;
}
