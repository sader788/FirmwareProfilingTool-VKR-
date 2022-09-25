#include <iostream>
#include "GuiWindow.h"

int APIENTRY wWinMain(
        HINSTANCE instance,
        HINSTANCE previousInstance,
        PWSTR arguments,
        int commandShow
) {

    GuiWindow window("FirmwareProfilingTool", 1250, 600);

    window.render();

    return EXIT_SUCCESS;
}
