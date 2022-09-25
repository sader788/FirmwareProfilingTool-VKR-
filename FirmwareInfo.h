//
// Created by 79110 on 22.05.2022.
//

#ifndef IMGUI_EXAMPLE_FIRMWAREINFO_H
#define IMGUI_EXAMPLE_FIRMWAREINFO_H

#include <iostream>
#include <ctime>
#include <cstdlib>
#include "WinTools.h"

class FirmwareInfo {
private:
    // vendor info
    char vendorName[32];
    char modelName[64];
    char firmwareVersion[64];
    char firmwareDate[64];
    // fs info
    char fsList[1024];
    // compressed info (zip, lzma, etc)
    char dataList[4096];
    // crypt info
    char cryptInfo[1024];
    // bin file info
    char fileName[128];
    time_t addedDate;

    int setSquahFS(char* buffer, int fsPnt, int seqMode);
    void setZIP(std::string& buffer, int dataPnt);
public:
    FirmwareInfo(char* filePath);

    char* getFileName();
    char* getFsPointer();
    char* getDataPointer();
    void setFsList(char* buffer);
    void setDataList(std::string& buffer);
};


#endif //IMGUI_EXAMPLE_FIRMWAREINFO_H
