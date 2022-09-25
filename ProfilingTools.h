//
// Created by 79110 on 22.05.2022.
//

#ifndef IMGUI_EXAMPLE_PROFILINGTOOLS_H
#define IMGUI_EXAMPLE_PROFILINGTOOLS_H

#include <iostream>
#include <sstream>
#include "FirmwareInfo.h"
#include "WinTools.h"


class ProfilingTools {
private:

    static void writeBufferIntToByte (char* buffer, int offset, int num, int size);
    static void checkJFFS2(char* buffer, int seqMode, char* fwBuffer);
    static void checkSquashFS(char* buffer, int seqMode, char* fwBuffer, int fileOffset);
    static void checkLZMA(char* buffer, char* fwBuffer);
    static void checkZIP(char* buffer, char* fwBuffer, int fileOffset);
public:
    static void scanFirmware(char* buffer, int size, FirmwareInfo* fw);
};


#endif //IMGUI_EXAMPLE_PROFILINGTOOLS_H
