#pragma once

#include <dwmapi.h>
#include <d3d11.h>
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include "memory.hpp"
#include "globals.h"

#pragma comment(lib, "d3d11.lib")

class Overlay
{
public:
	bool CreateOverlay();
	void DestroyOverlay();
	void OverlayLoop(HANDLE hDriver);
	void OverlayManager();

private:
	bool ShowMenu = false;

	// Overlay
	WNDCLASSEXA wc;  // A WNDCLASSEXA structure that defines the properties of the window class used to create the overlay window.
	HWND Hwnd;  // Handle to the overlay window.
	char ClassName[16] = "NULL";  // Name of the overlay window.
	char TitleName[16] = "";  // Title of the overlay window
};

// Extern refers to global variables.
extern ID3D11Device* g_pd3dDevice;  // The device interface represents a virtual adapter; it is used to create resources.
extern ID3D11DeviceContext* g_pd3dDeviceContext;  // The device context which generates rendering commands.
extern IDXGISwapChain* g_pSwapChain;  // The swapchain implements one or more surfaces for storing redered data before presenting it to an output.
extern ID3D11RenderTargetView* g_mainRenderTargetView;  // A render-target-view interface identifies the render-target subresources that can be accessed during rendering.