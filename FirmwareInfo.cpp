//
// Created by 79110 on 22.05.2022.
//

#include "FirmwareInfo.h"

FirmwareInfo::FirmwareInfo(char *filePath) {

    int j = strlen(filePath);

    while(filePath[j-1] != '\\') j--;
    strcpy (fileName, &filePath[j]);

    addedDate = time(0);

    memset(fsList, 0, 1024);
    memset(dataList, 0, 4096);
}

char * FirmwareInfo::getFileName() {
    return fileName;
}

char* FirmwareInfo::getFsPointer() {
    return fsList;
}
char* FirmwareInfo::getDataPointer() {
    return dataList;
}

int  FirmwareInfo::setSquahFS(char* buffer, int fsPnt, int seqMode){
    short compressionID = WinTools::convertBytesToInt (&fsList[fsPnt + 16], 2, BE_SEQ);
    long blockSize = WinTools::convertBytesToInt (&fsList[fsPnt + 12], 4, BE_SEQ);
    long inodeCount = WinTools::convertBytesToInt (&fsList[fsPnt + 8], 4, BE_SEQ);
    long offset = WinTools::convertBytesToInt (&fsList[fsPnt + 4], 4, BE_SEQ);


    sprintf(buffer,"SquashFS file system, %s version: 4, offs: %x, info: inode count = %i, block size = %i, compression id = %i\n",
            (seqMode == LE_SEQ) ? "little-endian" : "big-endian",offset, inodeCount, blockSize, compressionID);
    return strlen(buffer);

};

void  FirmwareInfo::setZIP(std::string& buffer, int dataPnt){

    char tmpBuffer[256];
    char path[48];

    memset(tmpBuffer, 0, 256);
    memset(tmpBuffer, 0, 48);

    long dictionarydSize = WinTools::convertBytesToInt (&dataList[dataPnt + 8], 4, BE_SEQ);
    long uncomressedSize = WinTools::convertBytesToInt (&dataList[dataPnt + 12], 4, BE_SEQ);
    long offset = WinTools::convertBytesToInt (&dataList[dataPnt + 4], 4, BE_SEQ);

    strncpy(path, &dataList[dataPnt + 16], 47);



    sprintf(tmpBuffer,"Zip compressed data, offs: %x, info: dict_size = %i, uncomp_size = %i, path = %s\n", offset, dictionarydSize, uncomressedSize, path);

    buffer += tmpBuffer;
};

void FirmwareInfo::setFsList(char *buffer) {
    short fsCount = WinTools::convertBytesToInt (fsList, 2, BE_SEQ);
    int bufferPnt = 0;

    memset(buffer, 0, 1024);

    for(int i = 2; i < (fsCount * 18) + 2; i += 18){
        if(!strncmp(&fsList[i], "SQLE", 4))
            bufferPnt = setSquahFS(&buffer[bufferPnt], i, LE_SEQ);


    }
}

void FirmwareInfo::setDataList(std::string& buffer) {
    short dataCount = WinTools::convertBytesToInt (dataList, 2, BE_SEQ);
    int bufferPnt = 0;

    buffer.clear();

    for(int i = 2; i < (dataCount * 64) + 2; i += 64
            ){
        if(!strncmp(&dataList[i], "ZIPA", 4))
            setZIP(buffer, i);


    }
}