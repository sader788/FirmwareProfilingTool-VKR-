//
// Created by ikmokhirio on 13.03.2022.
//

#ifndef CS_GO_CHEAT_TEST_GUIWINDOW_H
#define CS_GO_CHEAT_TEST_GUIWINDOW_H
#pragma once
#include <d3d9.h>
#include <string>
#include <windows.h>
#include <vector>
#include <fstream>
#include "WinTools.h"
#include "FirmwareInfo.h"
#include "ProfilingTools.h"

class GuiWindow {
private:
    static int width;
    static int height;
    static bool isRunning;

    static std::vector<FirmwareInfo> firmwareInfoList;

    std::string windowName;

    void createHWindow();
    void destroyHWindow();

    bool createDevice();
    void destroyDevice();

    void createImGui();
    void destroyImGui();

    void beginRender();
    void draw();
    void endRender();
    void resetDevice();

    static long __stdcall WindowProcess(
            HWND window,
            UINT message,
            WPARAM wideParameter,
            LPARAM longParameter);

    // WIN
    static HWND window;
    static POINTS position;
    WNDCLASSEX windowClass = { };

    // D3D
    PDIRECT3D9 d3d = nullptr;
    LPDIRECT3DDEVICE9 device = nullptr;
    D3DPRESENT_PARAMETERS presentParameters = { };

    static char fsList[1024];
    static std::string dataList;

public:

    GuiWindow(std::string name, int w, int h);

    ~GuiWindow();

    void render();

};

#endif //CS_GO_CHEAT_TEST_GUIWINDOW_H
