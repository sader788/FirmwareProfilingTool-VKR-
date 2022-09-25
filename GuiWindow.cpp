//
// Created by ikmokhirio on 13.03.2022.
//

#include "GuiWindow.h"

#include <imgui.h>
#include <iostream>

#include <utility>
#include <thread>
#include "../imgui/examples/imgui_impl_dx9.h"
#include "../imgui/examples/imgui_impl_win32.h"

HWND GuiWindow::window = nullptr;
POINTS GuiWindow::position = {};
int GuiWindow::width = 640;
int GuiWindow::height = 480;
bool GuiWindow::isRunning = false;
char GuiWindow::fsList[1024];
std::string GuiWindow::dataList;

std::vector<FirmwareInfo> GuiWindow::firmwareInfoList;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
        HWND window,
        UINT message,
        WPARAM wideParameter,
        LPARAM longParameter
);


GuiWindow::GuiWindow(std::string name, int w, int h) {
    windowName = std::move(name);
    width = w;
    height = h;

    isRunning = true;

    memset(fsList, 0, 1024);

    createHWindow();
    createDevice();
    createImGui();
}

void GuiWindow::createHWindow() {
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = WindowProcess;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleA(0);
    windowClass.hIcon = 0;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = "class001";
    windowClass.hIconSm = 0;

    RegisterClassEx(&windowClass);

    window = CreateWindowEx(
            0,
            "class001",
            windowName.c_str(),
            WS_POPUP,
            100,
            100,
            GuiWindow::width,
            GuiWindow::height,
            0,
            0,
            windowClass.hInstance,
            0
    );

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

bool GuiWindow::createDevice() {
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            window,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &presentParameters,
            &device) < 0)
        return false;

    return true;
}

void GuiWindow::createImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ::ImGui::GetIO();

    io.IniFilename = NULL;

    //ImGui::StyleColorsDark();

    ImGui::StyleColorsLight();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void GuiWindow::destroyImGui() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void GuiWindow::destroyDevice() {
    if (device) {
        device->Release();
        device = nullptr;
    }

    if (d3d) {
        d3d->Release();
        d3d = nullptr;
    }
}

void GuiWindow::destroyHWindow() {
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

void GuiWindow::beginRender() {
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT) {
            isRunning = false;
            return;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void GuiWindow::render() {
    while (isRunning) {

        beginRender();

        // DRAW
        draw();

        endRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void GuiWindow::endRender() {
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0) {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        resetDevice();
}

void GuiWindow::resetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

GuiWindow::~GuiWindow() {
    isRunning = false;
    destroyImGui();
    destroyDevice();
    destroyHWindow();
}

long GuiWindow::WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) {
        return true;
    }


    switch (message) {

        case WM_SYSCOMMAND: {
            if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
        }
            break;

        case WM_LBUTTONDOWN: {
            GuiWindow::position = MAKEPOINTS(longParameter); // set click points
        }
            return 0;

        case WM_MOUSEMOVE: {
            if (wideParameter == MK_LBUTTON) {
                const auto points = MAKEPOINTS(longParameter);
                auto rect = ::RECT{};

                GetWindowRect(GuiWindow::window, &rect);

                rect.left += points.x - GuiWindow::position.x;
                rect.top += points.y - GuiWindow::position.y;

                if (GuiWindow::position.x >= 0 &&
                    GuiWindow::position.x <= GuiWindow::width &&
                    GuiWindow::position.y >= 0 && GuiWindow::position.y <= 19)
                    SetWindowPos(
                            GuiWindow::window,
                            HWND_TOPMOST,
                            rect.left,
                            rect.top,
                            0, 0,
                            SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                    );
            }

        }
            return 0;

        case WM_DESTROY: {
            PostQuitMessage(0);
        }

            return 0;

    }

    return DefWindowProc(window, message, wideParameter, longParameter);

}

void GuiWindow::draw() {
    if(!isRunning) return;

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({(float) GuiWindow::width, (float) GuiWindow::height});
    ImGui::Begin(
            windowName.c_str(),
            &isRunning,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_MenuBar
    );

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New file")) {
                char binFilePathCP[512];
                LPWSTR binFilePathWP = WinTools::chooseFile();

                if (binFilePathWP != NULL) {wcstombs(
                        binFilePathCP, binFilePathWP, 512);
                        firmwareInfoList.push_back(*new FirmwareInfo(binFilePathCP));


                    std::ifstream in(binFilePathCP, std::ios::binary);

                    int size = in.seekg(0, std::ios::end).tellg();
                    in.seekg(0);

                    char* fwBuffer = (char*) malloc(size + 1);

                    in.read(fwBuffer, size);

                    ProfilingTools::scanFirmware(fwBuffer, size, &firmwareInfoList[firmwareInfoList.size() - 1]);

                    in.close();
                    free(fwBuffer);
                }
            }
            if (ImGui::MenuItem("Close")) isRunning = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Sort"))
        {
            if (ImGui::MenuItem("By name")) {
                char binFilePathCP[512];
                LPWSTR binFilePathWP = WinTools::chooseFile();

                if (binFilePathWP != NULL) {wcstombs(
                            binFilePathCP, binFilePathWP, 512);
                    firmwareInfoList.push_back(*new FirmwareInfo(binFilePathCP));
                }
            }
            if (ImGui::MenuItem("By added date")) isRunning = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    // Here

    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(200, 0), true);

    for(int i = 0; i < firmwareInfoList.size(); i++){
        if (ImGui::Selectable(firmwareInfoList[i].getFileName(), selected == i))
            selected = i;
    }

    ImGui::EndChild();
    ImGui::SameLine();

    // right
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
    if(firmwareInfoList.size() != 0){
        ImGui::Text("%s:", firmwareInfoList[selected].getFileName());
        firmwareInfoList[selected].setFsList(fsList);
        firmwareInfoList[selected].setDataList(dataList);
    }

    else
        ImGui::Text(" ");
    ImGui::Separator();
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Vendor Info"))
        {
            ImGui::TextWrapped("Vendor: TP-Link\nModel: MR6400\nVersion: 5.1.1.0.0.9.1.210803\nBuild date: 2021.08.05");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("System info"))
        {
            ImGui::Text(fsList);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("File Systems"))
        {
            ImGui::Text("Squashfs filesystem, offs:0x160200, info: little endian, version 4.0, 664 inodes, blocksize: 131072 bytes");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Compressed data"))
        {


            ImGui::Text(dataList.c_str());
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Crypt info"))
        {
            ImGui::Text("ID: 0123456789");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
    if (ImGui::Button("Save to file")) {}
    ImGui::EndGroup();





    ImGui::End();
}
