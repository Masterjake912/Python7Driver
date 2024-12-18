#pragma once

#include <vector>
#include <string>
#include <thread>
#include <d3d11.h>
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include "globals.h"
#include "memory.hpp"
#include "client.dll.hpp"
#include "offsets.hpp"
#include "buttons.hpp"
#include "vector.hpp"
#include "playerinfo.hpp"
#include "overlay.hpp"

#pragma comment (lib, "User32.lib")

using namespace cs2_dumper::schemas::client_dll;
using namespace cs2_dumper::offsets::client_dll;
using namespace cs2_dumper::buttons;

typedef struct
{
	ImU32 R;
	ImU32 G;
	ImU32 B;
} RGB;

inline ImU32 Color(RGB color)
{
	return IM_COL32(color.R, color.G, color.B, 255);
}

class Render
{
public:
	uintptr_t mPlayerAddress;
	Vector3 mLocalOrigin;
	ViewMatrix_t mViewMatrix;
	uintptr_t mEntityList;
	int mLocalTeam;
	std::vector<PlayerInfo*> mFriendlyTeam;
	std::vector<PlayerInfo*> mEnemyTeam;
	void UpdateList(HANDLE hDriver, uintptr_t client);
	void RenderInfo();
	void RenderMenu();
	void RenderESP(HANDLE hDriver, Globals global);
private:
	void DrawLine(int x1, int y1, int x2, int y2, RGB color, int thickness)
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Color(color), thickness);
	}
	void DrawRect(int x, int y, int w, int h, RGB color, int thickness)
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), Color(color), 0, 0, thickness);
	}
	void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
	}
	void String(ImVec2 pos, ImColor color, const char* text)
	{
		Text(pos, color, text, text + strlen(text), 200, 0);
	}
	void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
	}
	void HealthBar(float x, float y, float w, float h, int value, int v_max)
	{
		// Draw the background bar (black with transparency)
		RectFilled(x, y, x + w, y + h, ImColor(0.f, 0.f, 0.f, 0.75f), 0.f, 0);

		// Calculate the height of the filled portion based on health
		float filledHeight = (h / float(v_max)) * float(value);

		// Draw the filled portion from the top down
		RectFilled(x, y + (h - filledHeight), x + w, y + h, ImColor(min(510 * (v_max - value) / v_max, 255), min(510 * value / v_max, 255), 25, 255), 0.0f, 0);
	}
};