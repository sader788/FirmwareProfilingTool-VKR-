cmake_minimum_required(VERSION 3.16)
project(IMGUI_EXAMPLE)

set(CMAKE_CXX_STANDARD 20)
# -static-libgcc -static-libstdc++
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

set(DXSDK_DIR "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")

set(BUILD_SHARED_LIBS 0)

#IMGUI
set(ROOT ${PROJECT_SOURCE_DIR}/ImGui)
set(IMGUI_PUBLIC_HEADERS ${ROOT}/imgui.h)
set(IMGUI_PRIVATE_HEADERS
        ${ROOT}/imgui_internal.h ${ROOT}/imconfig.h ${ROOT}/imstb_rectpack.h
        ${ROOT}/imstb_textedit.h ${ROOT}/imstb_truetype.h)
set(IMGUI_SOURCES ${ROOT}/imgui.cpp ${ROOT}/imgui_draw.cpp
        ${ROOT}/imgui_widgets.cpp ${ROOT}/imgui_demo.cpp)

#PLATFORM
set(PLATFORM_HEADERS ${ROOT}/examples/imgui_impl_dx9.h ${ROOT}/examples/imgui_impl_win32.h)
set(PLATFORM_SOURCE ${ROOT}/examples/imgui_impl_dx9.cpp ${ROOT}/examples/imgui_impl_win32.cpp)

add_library(imgui_static STATIC ${IMGUI_SOURCES} ${IMGUI_PUBLIC_HEADERS}
        ${IMGUI_PRIVATE_HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCE})

target_include_directories(imgui_static PUBLIC ${ROOT})

if(BUILD_SHARED_LIBS)
    message("Shared")
    add_library(imgui_shared SHARED ${IMGUI_SOURCES})
    target_include_directories(imgui_shared PUBLIC ${ROOT})
    add_library(imgui ALIAS imgui_shared)
else()
    message("No shared")
    add_library(imgui ALIAS imgui_static)
endif()
#END

include_directories(${DXSDK_DIR}/Include)

if (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64 bits
        message("Using x64")
        link_directories(${DXSDK_DIR}/Lib/64)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        # 32 bits
        message("Using x86")
        link_directories(${DXSDK_DIR}/Lib/x86)
    endif ()
endif ()


add_executable(${PROJECT_NAME} WIN32 main.cpp GuiWindow.h GuiWindow.cpp WinTools.h WinTools.cpp FirmwareInfo.h FirmwareInfo.cpp ProfilingTools.h ProfilingTools.cpp)

target_link_libraries(${PROJECT_NAME} d3d9 imgui)