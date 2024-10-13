#pragma once
#include <Windows.h>

struct Globals
{
	// System
	bool Run = false;
	int MenuKey = VK_INSERT;  // Key to open Menu

	// Status
	bool ShowMenu = false;  // Is menu showing or not
	RECT GameSize{};
	POINT GamePoint{};

	// Visual
	bool ESP = true;
	bool ExfilESP = true;
	bool ESP_Box = true;
	bool ESP_Line = true;
	bool ESP_Team = true;  // Show players team
	bool ESP_Healthbar = true;
	bool ESP_Distance = true;

	// Miscellaneous
	bool Bhop = true;
	
};

extern struct Globals g;
