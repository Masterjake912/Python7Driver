#include <atomic>
#include <future>
#include <iostream>
#include "functionality.hpp"
#include "memory.hpp"
#include "overlay.hpp"
#include "render.hpp"

using namespace std;

Render Rd = Render();
Overlay Ov = Overlay();

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

void Overlay::OverlayLoop(HANDLE hDriver)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Rd.RenderInfo();

	if (ShowMenu)
		Rd.RenderMenu();

	if (g.ESP)
		Rd.RenderESP(hDriver, g);

	ImGui::Render();
	const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	g_pSwapChain->Present(0, 0);
}

int main()
{
	const DWORD pid = MyGetProcessId(L"cs2.exe");
	if (pid == 0)
	{
		cout << "Failed to get CS2 pid." << endl;
		cin.get();
		return 1;
	}

	const HANDLE driver = CreateFileW(L"\\\\.\\Python7", GENERIC_READ, 0, nullptr,
									  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (driver == INVALID_HANDLE_VALUE)
	{
		cout << " Failed to create a handle to the driver." << endl;
		cin.get();
		return 1;
	}

	if (driver::AttachToProcess(driver, pid) == true)
	{
		const uintptr_t client = GetModuleBase(pid, L"client.dll");
		if (client)
		{
			if (!Ov.CreateOverlay())
				return 2;
			g.Run = true;
			while (g.Run) {
				Rd.UpdateList(driver, client);
				Ov.OverlayManager();
				Ov.OverlayLoop(driver);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			/*std::thread([&]() { Rd.UpdateList(driver, client); }).detach();*/
			
			/*std::thread([&]() { Ov.OverlayManager(); }).detach();*/
			
		}
	}
}