//
// Created by 79110 on 22.05.2022.
//

#include "ProfilingTools.h"

void ProfilingTools::writeBufferIntToByte (char* buffer, int offset, int num, int size){

    char bytes[4];

    for (int i = 0; i < size; i++)
        bytes[size - 1 - i] = (num >> (i * 8));

    for (int i = offset, j = 0; i < offset + size; i++, j++)
        buffer[i] = bytes[j];
}

void ProfilingTools::checkJFFS2(char* buffer, int seqMode, char* fwBuffer){

    char fsName[5] = "JFS2";

    long jump = WinTools::convertBytesToInt (&buffer[4], 4, seqMode);

    if(buffer[2] == '\x01' && buffer[3] == '\xE0' && jump > 0 && seqMode == LE_SEQ
       || buffer[2] == '\xE0' && buffer[3] == '\x01' && jump > 0 && seqMode == BE_SEQ)
        short count = WinTools::convertBytesToInt(fwBuffer, 2, BE_SEQ);
}

void ProfilingTools::checkLZMA(char* buffer, char* fwBuffer){

    char lzmaName[5] = "LZMP";

    lzmaName[3] = buffer[0];

    long dictionarydSize = WinTools::convertBytesToInt (&buffer[1], 4, LE_SEQ);
    long uncomressedSize = WinTools::convertBytesToInt (&buffer[9], 4, LE_SEQ);
    // 4 + 4 + 4 + 32 = 44
    if(uncomressedSize != -1 && uncomressedSize >= 32){
        short count = WinTools::convertBytesToInt(fwBuffer, 2, BE_SEQ);
        short offset = (count * 44) + 2;

        writeBufferIntToByte(fwBuffer, 0, count + 1, 2);
        memcpy(&fwBuffer[offset], lzmaName, 4);
        writeBufferIntToByte(fwBuffer, offset + 4, dictionarydSize , 4);
        writeBufferIntToByte(fwBuffer, offset + 8, uncomressedSize , 4);
    }

}


void ProfilingTools::checkZIP(char* buffer, char* fwBuffer, int fileOffset){

    char zipName[5] = "ZIPA";
    char namePath[32];

    long compressedSize = WinTools::convertBytesToInt (&buffer[18], 4, LE_SEQ);
    long uncomressedSize = WinTools::convertBytesToInt (&buffer[22], 4, LE_SEQ);

    short len = WinTools::convertBytesToInt (&buffer[26], 2, LE_SEQ);
    // 4 + 4 + 4 + 32 = 44
    if(uncomressedSize != -1 && uncomressedSize >= 32 && compressedSize > 0){
        short count = WinTools::convertBytesToInt(fwBuffer, 2, BE_SEQ);
        short offset = (count * 64) + 2;

        writeBufferIntToByte(fwBuffer, 0, count + 1, 2);
        memcpy(&fwBuffer[offset], zipName, 4);
        writeBufferIntToByte(fwBuffer, offset + 4, fileOffset , 4);
        writeBufferIntToByte(fwBuffer, offset + 8, compressedSize , 4);
        writeBufferIntToByte(fwBuffer, offset + 12, uncomressedSize , 4);
        memcpy(&fwBuffer[offset + 16], &buffer[30], len);


    }

}

void ProfilingTools::checkSquashFS(char* buffer, int seqMode, char* fwBuffer, int fileOffset){
    char fsName[5] = "SQLE";

    if(seqMode == BE_SEQ)
        fsName[2] = 'B';

    short versionMinor = WinTools::convertBytesToInt (&buffer[30], 2, seqMode);
    short versionMajor = WinTools::convertBytesToInt (&buffer[28], 2, seqMode);
    short compressionID = WinTools::convertBytesToInt (&buffer[20], 2, seqMode);
    long blockSize = WinTools::convertBytesToInt (&buffer[12], 4, seqMode);
    long inodeCount = WinTools::convertBytesToInt (&buffer[4], 4, seqMode);

    if(versionMajor == 4 && versionMinor == 0
       && compressionID > 0 && compressionID <= 6 && inodeCount > 0){
        short count = WinTools::convertBytesToInt(fwBuffer, 2, BE_SEQ);
        short offset = (count * 18) + 2;

        writeBufferIntToByte(fwBuffer, 0, count + 1, 2);
        memcpy(&fwBuffer[offset], fsName, 4);
        writeBufferIntToByte(fwBuffer, offset + 4, fileOffset , 4);
        writeBufferIntToByte(fwBuffer, offset + 8, inodeCount , 4);
        writeBufferIntToByte(fwBuffer, offset + 12, blockSize, 4);
        writeBufferIntToByte(fwBuffer, offset + 16, compressionID, 2);

    }

}

void ProfilingTools::scanFirmware(char* buffer, int size, FirmwareInfo* fw){
    for(int i = 0; i < size; i++){
        if (buffer[i] == '\x19' && i > 0){
            if(buffer[i-1] == '\x85'){
                //checkJFFS2(&buffer[i], BE_SEQ)
            }
        }
        if (buffer[i] == '\x19' && i < size - 1){
            if(buffer[i+1] == '\x85'){
                //checkJFFS2(&buffer[i], BE_SEQ)
            }
        }
        if (buffer[i] == '\x73' || buffer[i] == '\x68' && i < size - 68){
            if(buffer[i + 1] == '\x71' && buffer[i+2] == '\x73' && buffer[i+3] == '\x68')
                checkSquashFS(&buffer[i], BE_SEQ, fw->getFsPointer(), i);
            if(buffer[i + 1] == '\x73' && buffer[i+2] == '\x71' && buffer[i+3] == '\x73')
                checkSquashFS(&buffer[i], LE_SEQ, fw->getFsPointer(), i);
        }

        //if ( buffer[i] == '\x5D') {
        //    if(buffer[i + 1] == 0 && buffer[i+2] == 0)
        //        checkLZMA(&buffer[i], fw->getFsPointer());
        //}

        if (buffer[i] == '\x50') {
            if(buffer[i + 1] == '\x4B' && buffer[i+2] == '\x03' && buffer[i+3] == '\x04')
                checkZIP(&buffer[i], fw->getDataPointer(), i);
        }
    }
};