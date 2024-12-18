#include "image.h"
#include "render.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Render::UpdateList(HANDLE hDriver, uintptr_t client)
{
	mFriendlyTeam.clear();
	mEnemyTeam.clear();
	// Read local player's information (position, team, etc.)
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

	// Get Player info
	std::vector<PlayerInfo*> friendlyTeam;
	std::vector<PlayerInfo*> enemyTeam;
	for (int playerIndex = 1; playerIndex < 32; ++playerIndex) {
		uintptr_t listEntry = driver::ReadMemory<uintptr_t>(hDriver, mEntityList + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
		if (!listEntry)
			continue;

		uintptr_t player = driver::ReadMemory<uintptr_t>(hDriver, listEntry + 120 * (playerIndex & 0x1FF));
		if (!player)
			continue;

		int playerTeam = driver::ReadMemory<int>(hDriver, player + C_BaseEntity::m_iTeamNum);

		uint32_t playerPawn = driver::ReadMemory<uint32_t>(hDriver, player + CCSPlayerController::m_hPlayerPawn);
		uintptr_t listEntry2 = driver::ReadMemory<uintptr_t>(hDriver, mEntityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
		if (!listEntry2)
			continue;

		uintptr_t pCSPlayerPawn = driver::ReadMemory<uintptr_t>(hDriver, listEntry2 + 120 * (playerPawn & 0x1FF));
		if (!pCSPlayerPawn)
			continue;

		int health = driver::ReadMemory<int>(hDriver, pCSPlayerPawn + C_BaseEntity::m_iHealth);
		Vector3 origin = driver::ReadMemory<Vector3>(hDriver, pCSPlayerPawn + C_BasePlayerPawn::m_vOldOrigin);
		Vector3 head = { origin.x, origin.y, origin.z + 75.f };
		Vector3 screenPos = origin.WorldToScreen(mViewMatrix);
		Vector3 screenHead = head.WorldToScreen(mViewMatrix);

		PlayerInfo* playerInfo = new PlayerInfo(listEntry, player, playerTeam, playerPawn, listEntry2, pCSPlayerPawn, health, origin, head, screenPos, screenHead);
		if (playerTeam == localTeam)
		{
			friendlyTeam.push_back(playerInfo);
		} else {
			enemyTeam.push_back(playerInfo);
		}
	}
	mFriendlyTeam = friendlyTeam;
	mEnemyTeam = enemyTeam;
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
	if (global.ESP_Team)
	{
		for (std::vector<PlayerInfo*>::iterator it = mFriendlyTeam.begin(); it != mFriendlyTeam.end(); ++it) {
			PlayerInfo* playerInfo = *it;

			if (!playerInfo)
				continue;

			if (playerInfo->mPCSPlayerPawn == mPlayerAddress)
				continue;

			if (playerInfo->mHealth <= 0 || playerInfo->mHealth > 100)
				continue;

			float height = playerInfo->mScreenPos.y - playerInfo->mScreenHead.y;
			float width = height / 2.4f;

			int rectBottomX = playerInfo->mScreenHead.x;
			int rectBottomY = playerInfo->mScreenHead.y + height;

			int bottomCenterX = screenWidth / 2;
			int bottomCenterY = screenHeight;

			if (playerInfo->mScreenHead.x - width / 2 >= 0 &&
				playerInfo->mScreenHead.x + width / 2 <= screenWidth &&
				playerInfo->mScreenHead.y >= 0 &&
				playerInfo->mScreenHead.y + height <= screenHeight &&
				playerInfo->mScreenHead.z > 0 &&
				global.ESP == true) {
				// teammates with team color
				if (global.ESP_Line)
					Render::DrawLine(bottomCenterX, bottomCenterY, rectBottomX, rectBottomY, team, 1.5f);
				if (global.ESP_Box)
					Render::DrawRect(playerInfo->mScreenHead.x - width / 2, playerInfo->mScreenHead.y, width, height, team, 1.5);
				if (global.ESP_Healthbar)
					Render::HealthBar(playerInfo->mScreenHead.x - width / 2, playerInfo->mScreenHead.y, 2, height, playerInfo->mHealth, 100);
			}
		}
	}
	for (std::vector<PlayerInfo*>::iterator it = mEnemyTeam.begin(); it != mEnemyTeam.end(); ++it) {
		PlayerInfo* playerInfo = *it;

		if (!playerInfo)
			continue;

		if (!global.ESP_Team) {
			if (playerInfo->mPlayerTeam == mLocalTeam)
				continue;
		}

		if (playerInfo->mPCSPlayerPawn == mPlayerAddress)
			continue;

		if (playerInfo->mHealth <= 0 || playerInfo->mHealth > 100)
			continue;

		float height = playerInfo->mScreenPos.y - playerInfo->mScreenHead.y;
		float width = height / 2.4f;

		int rectBottomX = playerInfo->mScreenHead.x;
		int rectBottomY = playerInfo->mScreenHead.y + height;

		int bottomCenterX = screenWidth / 2;
		int bottomCenterY = screenHeight;

		if (playerInfo->mScreenHead.x - width / 2 >= 0 &&
			playerInfo->mScreenHead.x + width / 2 <= screenWidth &&
			playerInfo->mScreenHead.y >= 0 &&
			playerInfo->mScreenHead.y + height <= screenHeight &&
			playerInfo->mScreenHead.z > 0 &&
			global.ESP == true) {
			// enemies with enemy color
			if (global.ESP_Line)
				Render::DrawLine(bottomCenterX, bottomCenterY, rectBottomX, rectBottomY, enemy, 1.5f);
			if (global.ESP_Box)
				Render::DrawRect(playerInfo->mScreenHead.x - width / 2, playerInfo->mScreenHead.y, width, height, enemy, 2.5);
			if (global.ESP_Healthbar)
				Render::HealthBar(playerInfo->mScreenHead.x - width / 2, playerInfo->mScreenHead.y, 2, height, playerInfo->mHealth, 100);
		}
	}
}

void Render::RenderInfo()
{
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(8.f, 8.f), ImColor(1.f, 1.f, 1.f, 1.f), std::to_string((int)ImGui::GetIO().Framerate).c_str());
}

void Render::RenderMenu()
{
	/* LOAD MENU IMAGE START */
	unsigned char* rgba_data = stbi_load_from_memory(rawData, sizeof(rawData), &imageWidth, &imageHeight, &channels, 4);
	ID3D11ShaderResourceView* myTexture = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = imageWidth;
	desc.Height = imageHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = rgba_data;
	subResource.SysMemPitch = imageWidth * 4;

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	if (SUCCEEDED(hr)) {
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &myTexture);
		pTexture->Release();
	}
	stbi_image_free(rgba_data);
	/* LOAD MENU IMAGE END */

	// Set the window size and flags
	ImGui::SetNextWindowSize(ImVec2(380.f, 250.f));
	ImGui::Begin("##Menu", &g.ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	// Image in the background
	ImGui::Image((void*)myTexture, ImVec2(imageWidth, imageHeight));

	// Create a horizontal layout for the left-side tab bar
	ImGui::BeginChild("LeftPanel", ImVec2(30.f, 0), true); // Fixed width for the left panel

	// Define a custom style for vertical tabs (buttons)
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));  // Vertical spacing between tabs
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0)); // Remove extra inner space between buttons

	if (ImGui::Button("ESP")) {
		g.ActiveTab = 0;
	}
	if (ImGui::Button("Misc")) {
		g.ActiveTab = 1;
	}

	ImGui::PopStyleVar(2);  // Reset style variables

	ImGui::EndChild(); // End Left Panel for the buttons (tabs)

	// Right content panel (displayed when tabs are selected)
	ImGui::SameLine(0.0f, 0.0f); // No extra space between panels
	ImGui::BeginChild("RightPanel", ImVec2(0, 0), true); // The content displayed on the right

	// Show content based on active tab
	if (g.ActiveTab == 0) {
		// Content for "ESP"
		ImGui::Text("ESP Options");
		ImGui::Separator();
		ImGui::Checkbox("ESP", &g.ESP);
		ImGui::Checkbox("ESP Box", &g.ESP_Box);
		ImGui::Checkbox("ESP Line", &g.ESP_Line);
		ImGui::Checkbox("Show Team Boxes", &g.ESP_Team);
		ImGui::Checkbox("ESP Healthbar", &g.ESP_Healthbar);
	}
	else if (g.ActiveTab == 1) {
		// Content for "Miscellaneous"
		ImGui::Text("Miscellaneous");
		ImGui::Separator();
		// ImGui::Checkbox("Bunny Hops", &g.Bhop); // Add your checkbox content
	}

	ImGui::EndChild(); // End Right Panel for the content

	ImGui::End();
}
