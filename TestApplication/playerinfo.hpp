#pragma once

class PlayerInfo
{
public:
	uintptr_t mListEntry;
	uintptr_t mPlayer;
	int mPlayerTeam;
	uint32_t mPlayerPawn;
	uintptr_t mListEntry2;
	uintptr_t mPCSPlayerPawn;
	int mHealth;
	Vector3 mOrigin;
	Vector3 mHead;
	Vector3 mScreenPos;
	Vector3 mScreenHead;

	// Constructor
	PlayerInfo(uintptr_t listEntry, uintptr_t player, int playerTeam, uint32_t playerPawn,
		uintptr_t listEntry2, uintptr_t pCSPlayerPawn, int health, Vector3 origin, Vector3 head,
		Vector3 screenPos, Vector3 screenHead)
	{
		mListEntry = listEntry;
		mPlayer = player;
		mPlayerTeam = playerTeam;
		mPlayerPawn = playerPawn;
		mListEntry2 = listEntry2;
		mPCSPlayerPawn = pCSPlayerPawn;
		mHealth = health;
		mOrigin = origin;
		mHead = head;
		mScreenPos = screenPos;
		mScreenHead = screenHead;
	}

	// Default Constructor
	PlayerInfo() = default;
};