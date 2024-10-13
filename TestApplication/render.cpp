#include "render.hpp"

void Render::UpdateList(HANDLE hDriver, uintptr_t client)
{
	// Initialize bool's
	bool bhopEnabled = false;

	while (g.Run)
	{
		uintptr_t playerAddress = driver::ReadMemory<uintptr_t>(hDriver, client + dwLocalPlayerPawn);
		Vector3 localOrigin = driver::ReadMemory<Vector3>(hDriver, playerAddress + C_BasePlayerPawn::m_vOldOrigin);
		ViewMatrix_t viewMatrix = driver::ReadMemory<ViewMatrix_t>(hDriver, client + dwViewMatrix);
		uintptr_t entityList = driver::ReadMemory<uintptr_t>(hDriver, client + dwEntityList);
		int localTeam = driver::ReadMemory<int>(hDriver, playerAddress + C_BaseEntity::m_iTeamNum);
		mPlayerAddress = playerAddress;
		mLocalOrigin = localOrigin;
		mViewMatrix = viewMatrix;
		mEntityList = entityList;
		mLocalTeam = localTeam;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
}

void Render::RenderInfo()
{
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(8.f, 8.f), ImColor(1.f, 1.f, 1.f, 1.f), std::to_string((int)ImGui::GetIO().Framerate).c_str());
}

void Render::RenderMenu()
{
	ImGui::SetNextWindowSize(ImVec2(300.f, 250.f));
	ImGui::Begin("Jake's Menu", &g.ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Visual");
	/*ImGui::Separator();
	ImGui::Checkbox("ESP", &g.ESP);
	ImGui::Checkbox("Exfil ESP", &g.ExfilESP);*/

	ImGui::NewLine();

	ImGui::Text("ESP Options");
	ImGui::Separator();
	ImGui::Checkbox("ESP", &g.ESP);
	ImGui::Checkbox("ESP Box", &g.ESP_Box);
	ImGui::Checkbox("ESP Line", &g.ESP_Line);
	ImGui::Checkbox("Show Team Boxes", &g.ESP_Team);
	//ImGui::Checkbox("ESP Healthbar", &g.ESP_Healthbar);
	//ImGui::Checkbox("ESP Distance", &g.ESP_Distance);

	ImGui::NewLine();

	ImGui::Text("Miscellanous");
	ImGui::Separator();
	//ImGui::Checkbox("Bunny Hops", &g.Bhop);
	/*ImGui::Checkbox("Infinite Ammo", &g.Ammo);
	ImGui::Checkbox("Infinite Armor", &g.Armor);
	ImGui::Checkbox("Randomize Name", &g.Name);*/


	ImGui::End();

}

void Render::RenderESP(HANDLE hDriver, Globals global)
{
	static int r = 255;
	static int g = 0;
	static int b = 255;
	static int t_r = 0;
	static int t_g = 160;
	static int t_b = 255;
	RGB enemy = { r, g, b };
	RGB team = { t_r, t_g, t_b };
	for (int playerIndex = 1; playerIndex < 32; ++playerIndex) {
		uintptr_t listEntry = driver::ReadMemory<uintptr_t>(hDriver, mEntityList + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
		if (!listEntry)
			continue;

		uintptr_t player = driver::ReadMemory<uintptr_t>(hDriver, listEntry + 120 * (playerIndex & 0x1FF));
		if (!player)
			continue;

		int playerTeam = driver::ReadMemory<int>(hDriver, player + C_BaseEntity::m_iTeamNum);
		if (!global.ESP_Team) {
			if (playerTeam == mLocalTeam)
				continue;
		}

		uint32_t playerPawn = driver::ReadMemory<uint32_t>(hDriver, player + CCSPlayerController::m_hPlayerPawn);
		uintptr_t listEntry2 = driver::ReadMemory<uintptr_t>(hDriver, mEntityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
		if (!listEntry2)
			continue;

		uintptr_t pCSPlayerPawn = driver::ReadMemory<uintptr_t>(hDriver, listEntry2 + 120 * (playerPawn & 0x1FF));
		if (!pCSPlayerPawn)
			continue;
		if (pCSPlayerPawn == mPlayerAddress)
			continue;

		int health = driver::ReadMemory<int>(hDriver, pCSPlayerPawn + C_BaseEntity::m_iHealth);
		if (health <= 0 || health > 100)
			continue;

		
		Vector3 origan = driver::ReadMemory<Vector3>(hDriver, pCSPlayerPawn + C_BasePlayerPawn::m_vOldOrigin);
		Vector3 head = { origan.x, origan.y, origan.z + 75.f };

		Vector3 screenPos = origan.WorldToScreen(mViewMatrix);
		Vector3 screenHead = head.WorldToScreen(mViewMatrix);

		float height = screenPos.y - screenHead.y;
		float width = height / 2.4f;

		int rectBottomX = screenHead.x;
		int rectBottomY = screenHead.y + height;

		int bottomCenterX = screenWidth / 2;
		int bottomCenterY = screenHeight;

		if (screenHead.x - width / 2 >= 0 &&
			screenHead.x + width / 2 <= screenWidth &&
			screenHead.y >= 0 &&
			screenHead.y + height <= screenHeight &&
			screenHead.z > 0 &&
			global.ESP == true) {

			if (playerTeam == mLocalTeam) {
				// teammates with team color
				if (global.ESP_Line)
					Render::DrawLine(bottomCenterX, bottomCenterY, rectBottomX, rectBottomY, team, 1.5f);
				if (global.ESP_Box)
					Render::DrawRect(screenHead.x - width / 2, screenHead.y, width,	height,	team, 1.5);
			}
			else {
				// enemies with enemy color
				if (global.ESP_Line)
					Render::DrawLine(bottomCenterX, bottomCenterY, rectBottomX, rectBottomY, enemy, 1.5f);
				if (global.ESP_Box)
					Render::DrawRect(screenHead.x - width / 2, screenHead.y, width, height, enemy, 1.5);
			}
		}
	}
	// GetLocal
	//Player* pLocal = mPlayer;
	//if (!pLocal->Update(false))
	//	return;

	//AssaultCube.UpdateViewMatrix();

	//// Player
	//for (Player& player : mEntityList)
	//{
	//	Player* pEntity = &player;
	//	if (!pEntity->Update(false))
	//		continue;

	//	float distance = GetDistance(pLocal->mBasePos, pEntity->mBasePos);

	//	Vector2 g_Screen{}, g_HeadScreen{}, g_NeckScreen{};
	//	if (!AssaultCube.WorldToScreen(pEntity->mBasePos, g_Screen) || !AssaultCube.WorldToScreen(pEntity->mHeadPos, g_HeadScreen) || !AssaultCube.WorldToScreen(pEntity->mNeckPos, g_NeckScreen))
	//		continue;

	//	float NeckToHead = (g_NeckScreen.y - g_HeadScreen.y) * 3.f;
	//	float pHeight = g_Screen.y - g_HeadScreen.y + NeckToHead;
	//	float pWidth = pHeight / 3.5f; // Half

	//	// Line
	//	if (g.ESP_Line)
	//		DrawLine(ImVec2(g.GameSize.right / 2.f, g.GameSize.bottom), ImVec2(g_Screen.x, g_Screen.y), ImColor(1.f, 0.f, 0.f, 0.f), 1.f);

	//	// 2D Box
	//	if (g.ESP_Box)
	//	{
	//		DrawLine(ImVec2(g_Screen.x - pWidth, g_HeadScreen.y - NeckToHead), ImVec2(g_Screen.x + pWidth, g_HeadScreen.y - NeckToHead), ImColor(1.f, 0.f, 0.f, 1.f), 1.f);
	//		DrawLine(ImVec2(g_Screen.x - pWidth, g_Screen.y), ImVec2(g_Screen.x - pWidth, g_HeadScreen.y - NeckToHead), ImColor(1.f, 0.f, 0.f, 1.f), 1.f);
	//		DrawLine(ImVec2(g_Screen.x + pWidth, g_Screen.y), ImVec2(g_Screen.x + pWidth, g_HeadScreen.y - NeckToHead), ImColor(1.f, 0.f, 0.f, 1.f), 1.f);
	//		DrawLine(ImVec2(g_Screen.x - pWidth, g_Screen.y), ImVec2(g_Screen.x + pWidth, g_Screen.y), ImColor(1.f, 0.f, 0.f, 1.f), 1.f);
	//	}

	//	// Healthbar
	//	if (g.ESP_Healthbar)
	//	{
	//		int health = m.Read<int>(pEntity->mHealthPtr);
	//		HealthBar(g_Screen.x - pWidth - 4.f, g_Screen.y, 2.f, -pHeight, health, 100);
	//	}


	//	// Distance
	//	if (g.ESP_Distance)
	//	{
	//		std::string DistStr = std::to_string((int)distance) + "m";
	//		String(ImVec2(g_Screen.x - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), g_Screen.y), ImColor(1.f, 1.f, 1.f, 1.f), DistStr.c_str());
	//	}
	//}
}