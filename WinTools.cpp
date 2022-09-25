//
// Created by 79110 on 22.05.2022.
//

#include "WinTools.h"

LPWSTR WinTools::chooseFile() {
    OPENFILENAMEW file;
    memset(&file, 0, sizeof(file));

    wchar_t fileSz[MAX_PATH];
    fileSz[0] = L'\0';
    file.lStructSize = sizeof(OPENFILENAMEW);
    file.hwndOwner = NULL;
//    file.lpstrFilter = L"All files (*.*)\0*.*";

    file.lpstrDefExt = L"\0";
    file.lpstrFile = fileSz;
    file.nMaxFile = MAX_PATH;
    file.lpstrTitle = L"Open dll file...";
    file.Flags = OFN_FILEMUSTEXIST;
    if (GetOpenFileNameW(&file) == TRUE) {
        return file.lpstrFile; // Return wide string path to file
    } else {
        return NULL;
    }

}

long WinTools::convertBytesToInt (char* buffer, int size, int seqMode) {

    long num = 0;

    if (seqMode == LE_SEQ)
        for (int i = 0; i < size; i++)
            num |= (unsigned char)(buffer[i]) << (i * 8);
    else if (seqMode == BE_SEQ)
        for (int i = 0, j = size - 1; i < size; i++, j--)
            num |= (unsigned char)(buffer[i]) << (j * 8);


    return num;
}