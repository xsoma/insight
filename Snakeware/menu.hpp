#pragma once

#include <string>
#include "singleton.hpp"
#include "imgui/imgui.h"
#include <d3dx9tex.h>
#pragma comment(lib,"d3dx9.lib")


struct IDirect3DDevice9;



class Menu
    : public Singleton<Menu>
{
public:
    void Initialize();
    void Shutdown();
    bool new_window;
    IDirect3DTexture9* smoke = nullptr;
    IDirect3DTexture9* model = nullptr;
    IDirect3DTexture9* model_flat = nullptr;
    IDirect3DTexture9* model_regular = nullptr;
    IDirect3DTexture9* model_met = nullptr;
    void OnDeviceLost();
    void OnDeviceReset();

    void Render();

    void Toggle();

    bool IsVisible() { return _visible; }

    ImGuiStyle        _style;
    bool              _visible;
private:
    void CreateStyle();

    
    
};
inline Menu g_menu;