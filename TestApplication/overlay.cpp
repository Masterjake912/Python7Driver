#include <thread>
#include "overlay.hpp"

ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

void CleanupRenderTarget();
void CleanupDeviceD3D();
void CreateRenderTarget();
bool CreateDeviceD3D(HWND hWnd);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool IsKeyDown(int VK)
{
	return (GetAsyncKeyState(VK) & 0x8000) != 0;
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = NULL;
    }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = NULL;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = NULL;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
}

void CreateRenderTarget()
{
    // Create a pointer to a ID3D11Texture2D object which holds the back buffer.
    ID3D11Texture2D* pBackBuffer;
    // Retrieves back buffer of the swap chain stores it in pBackBuffer.
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    // Creates a render target view for the back buffer and stores the newly created render target view in g_mainRenderTargetView.
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    // Releases back buffer since it is no longer needed.
    pBackBuffer->Release();
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;  // Swap chain will have two back buffers. Enables double buffering, avoids flickering.
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;  // 60 Hz
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // Allows swap chain to switch display modes.
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // Indicates back buffer will be used as a render target.
    sd.OutputWindow = hWnd;  // The Handle to the window where the swap chain will render
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;  // Windowed Mode
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;  // No special flags
    D3D_FEATURE_LEVEL featureLevel;
    // Feature levels the device should support. Attempt to create a device with the highest available feature level from the array.
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    // Function creates both the Direct3D device and the swap chain.
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;
    // Create render target view for the swap chain's back buffer
    CreateRenderTarget();
    return true;
}

bool Overlay::CreateOverlay()
{
    // This initializes a WNDCLASSEXA structure, which is used to register a window class. The structure is populated with the size of the structure, a pointer to a window procedure function (WndProc), and other window class properties.
    wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, TitleName, ClassName, NULL };

    // Registers the window class with the system using the WNDCLASSEXA structure. This step is necessary before creating a window.
    if (!RegisterClassExA(&wc))
    {
        return false;  // Failed to register window class
    }

    // Creates a window with the specified class name and styles. First and second 100 are position x,y. Third and fourth 100, 100 is size of window. Returns handle to window.
    Hwnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, wc.lpszClassName, wc.lpszMenuName, WS_POPUP | WS_VISIBLE, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL);
    if (!Hwnd)
    {
        return false;  // Failed to create window
    }

    // Sets the transparency color and opacity for the window. In this case, the window is fully opaque (255). Uses handle to previously created window.
    SetLayeredWindowAttributes(Hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    // Extends the frame of the window into the client area, making the window's borderless appearance blend with the client area.
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(Hwnd, &margin);

    // Calls CreateDeviceD3D to initialize DirectX 11. If Initialization fails, it cleans up resources and exits the application
    if (!CreateDeviceD3D(Hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        exit(0);
    }

    // Makes the window visible and updates its apperance
    ShowWindow(Hwnd, SW_SHOWDEFAULT);
    UpdateWindow(Hwnd);

    IMGUI_CHECKVERSION();  // Checks versin compatability.
    ImGui::CreateContext();  // Creates an ImGui context.
    //Configures settings for ImGui
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.LogFilename = nullptr;
    io.IniFilename = nullptr;

    // Initializes ImGui with the Win32 and DirectX 11 backends, allowing it to render UI elements using DircetX 11.
    if (!ImGui_ImplWin32_Init(Hwnd) || !ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext))
    {
        DestroyOverlay();  // Cleanup resources
        return false;
    }

    return true;  // Success
}

void Overlay::DestroyOverlay()
{
    ImGui_ImplDX11_Shutdown();  // Cleans up resources and states related to rendering ImGui interfaces using DirectX 11.
    ImGui_ImplWin32_Shutdown();  // Cleans up resources related to handling Windows messages for ImGui.
    ImGui::DestroyContext();  // Releases any resources associated with ImGui, including its internal state and configuration.

    CleanupDeviceD3D();  // Releases DirectX 11 resources and cleans up the DirectX 11 device and context.
    DestroyWindow(Hwnd);  // Cleans up the window handle and releases associated system resources.
    UnregisterClassA(wc.lpszClassName, wc.hInstance);  // Removes the class registration from the system, which is necessary after destroying the window to prevent resource leaks.

}

// FIX THIS FIND WINDOW A
void Overlay::OverlayManager()
{
    // Window Style Changer
    static LONG MenuStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
    static LONG ESPStyle = WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST;
    LONG TmpLong = GetWindowLong(Hwnd, GWL_EXSTYLE);  // Gets current menu style.
    HWND ForegroundWindow = GetForegroundWindow();  // Retrieves a handle to the foreground window (the window with which the user is currently working).
    // If ShowMenu is true and MenuStyle is not currently set...
    if (ShowMenu && MenuStyle != TmpLong)
    {
        // Sets the window style (GWL_EXSTYLE) to MenuStyle
        SetWindowLong(Hwnd, GWL_EXSTYLE, MenuStyle);

        // Sets overlay to foreground window
        if (ForegroundWindow != Hwnd) {
            SetForegroundWindow(Hwnd);
        }
    }
    else if (!ShowMenu && ESPStyle != TmpLong)
    {
        // Sets the window style (GWL_EXSTYLE) to ESPStyle
        SetWindowLong(Hwnd, GWL_EXSTYLE, ESPStyle);

        // Sets overlay to foreground window
        if (ForegroundWindow != Hwnd)
            SetForegroundWindow(Hwnd);
    }

    // Toggles the ShowMenu state when a specific key is pressed
    static bool menu_key = false;  // Debounces the key press to avoid multiple toggles from a single key press
    if (IsKeyDown(g.MenuKey) && !menu_key)
    {
        ShowMenu = !ShowMenu;

        menu_key = true;  // Debounces the key press to avoid multiple toggles from a single key press
    }
    else if (!IsKeyDown(g.MenuKey) && menu_key)
    {
        menu_key = false;  // Debounces the key press to avoid multiple toggles from a single key press
    }

    // Retrieves the dimensions and screen position of the target window
    RECT TmpRect = {};  // Size
    POINT TmpPoint = {};  // Position
    GetClientRect(Hwnd, &TmpRect);  // Get Size
    ClientToScreen(Hwnd, &TmpPoint);  // Get Position

    // Compares the values to g.GameSize and g.GamePoint and updates the overlay window's position and size to match the target windows
    if (TmpRect.left != g.GameSize.left || TmpRect.bottom != g.GameSize.bottom || TmpRect.top != g.GameSize.top || TmpRect.right != g.GameSize.right || TmpPoint.x != g.GamePoint.x || TmpPoint.y != g.GamePoint.y)
    {
        g.GameSize = TmpRect;
        g.GamePoint = TmpPoint;

        SetWindowPos(Hwnd, nullptr, TmpPoint.x, TmpPoint.y, g.GameSize.right, g.GameSize.bottom, SWP_NOREDRAW);
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Checks to see if ImGui needs to handle the message. 
    // Returns true if it did, allowing the function to exit early as it was already handled.
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:  // Message sent when the window is resized.
        // Check if the Direct3D device is valid and Ensure that the message is not sent when the window is minimized.
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            // Cleans up the current render target to prepare for resizing
            CleanupRenderTarget();

            // Resizes the swap chain buffers to match the new window size.
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

            // Creates a new render target for the resized window
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:  // Message is sent when a system command is requested.
        // This is a system command for the key menu, which is typically the Alt key. Returning 0 prevents the menu from being shown when the Alt key is pressed.
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:  // Message is sent when the window is being destroyed.
        PostQuitMessage(0);  // Posts a quit message to the message queue, signaling the application to terminate.
        return 0;  // Ends the message processing for this message
    }
    // Calls the default window procedure to handle messages that are not specifically handled by this function.
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}
