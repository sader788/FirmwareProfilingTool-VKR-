//
// Created by 79110 on 22.05.2022.
//

#ifndef IMGUI_EXAMPLE_WINTOOLS_H
#define IMGUI_EXAMPLE_WINTOOLS_H

#include <sstream>
#include "windows.h"

const int LE_SEQ = 0;
const int BE_SEQ = 1;

class WinTools {
public:
    static LPWSTR chooseFile();
    static long convertBytesToInt (char* buffer, int size, int seqMode);
};


#endif //IMGUI_EXAMPLE_WINTOOLS_H
