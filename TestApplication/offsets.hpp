// Generated using https://github.com/a2x/cs2-dumper
// 2024-12-16 03:10:51.946329600 UTC

#pragma once

#include <cstddef>

namespace cs2_dumper {
    namespace offsets {
        // Module: client.dll
        namespace client_dll {
            constexpr std::ptrdiff_t dwCSGOInput = 0x1A5E280;
            constexpr std::ptrdiff_t dwEntityList = 0x19F2488;
            constexpr std::ptrdiff_t dwGameEntitySystem = 0x1B0CCF8;
            constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x1520;
            constexpr std::ptrdiff_t dwGameRules = 0x1A4FE68;
            constexpr std::ptrdiff_t dwGlobalVars = 0x18499C0;
            constexpr std::ptrdiff_t dwGlowManager = 0x1A4F5A8;
            constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A41FD0;
            constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1855CE8;
            constexpr std::ptrdiff_t dwPlantedC4 = 0x1A59ED0;
            constexpr std::ptrdiff_t dwPrediction = 0x1855B80;
            constexpr std::ptrdiff_t dwSensitivity = 0x1A50B88;
            constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x40;
            constexpr std::ptrdiff_t dwViewAngles = 0x1A5E650;
            constexpr std::ptrdiff_t dwViewMatrix = 0x1A54550;
            constexpr std::ptrdiff_t dwViewRender = 0x1A54D60;
            constexpr std::ptrdiff_t dwWeaponC4 = 0x19F55F8;
        }
        // Module: engine2.dll
        namespace engine2_dll {
            constexpr std::ptrdiff_t dwBuildNumber = 0x531BE4;
            constexpr std::ptrdiff_t dwNetworkGameClient = 0x530CE0;
            constexpr std::ptrdiff_t dwNetworkGameClient_clientTickCount = 0x368;
            constexpr std::ptrdiff_t dwNetworkGameClient_deltaTick = 0x27C;
            constexpr std::ptrdiff_t dwNetworkGameClient_isBackgroundMap = 0x281447;
            constexpr std::ptrdiff_t dwNetworkGameClient_localPlayer = 0xF0;
            constexpr std::ptrdiff_t dwNetworkGameClient_maxClients = 0x238;
            constexpr std::ptrdiff_t dwNetworkGameClient_serverTickCount = 0x36C;
            constexpr std::ptrdiff_t dwNetworkGameClient_signOnState = 0x228;
            constexpr std::ptrdiff_t dwWindowHeight = 0x6140B4;
            constexpr std::ptrdiff_t dwWindowWidth = 0x6140B0;
        }
        // Module: inputsystem.dll
        namespace inputsystem_dll {
            constexpr std::ptrdiff_t dwInputSystem = 0x387E0;
        }
        // Module: matchmaking.dll
        namespace matchmaking_dll {
            constexpr std::ptrdiff_t dwGameTypes = 0x1A41B0;
            constexpr std::ptrdiff_t dwGameTypes_mapName = 0x120;
        }
        // Module: soundsystem.dll
        namespace soundsystem_dll {
            constexpr std::ptrdiff_t dwSoundSystem = 0x39A5E0;
            constexpr std::ptrdiff_t dwSoundSystem_engineViewData = 0x7C;
        }
    }
}
