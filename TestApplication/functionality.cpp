#include "functionality.hpp"

bool ToggleBhop(bool toggle, HANDLE driver, uintptr_t client) {
    if (toggle) {
        const auto localPlayerPawn = driver::ReadMemory<uintptr_t>(driver, client + client_dll::dwLocalPlayerPawn);
        if (localPlayerPawn == 0)
            return false;
        const auto flags = driver::ReadMemory<uint32_t>(driver, localPlayerPawn + C_BaseEntity::m_fFlags);
        const bool isInAir = flags & (1 << 0);
        const bool spacePressed = GetAsyncKeyState(VK_SPACE);
        const auto forceJump = driver::ReadMemory<DWORD>(driver, client + jump);

        if (spacePressed && isInAir) {
            Sleep(10);
            driver::WriteMemory(driver, client + jump, 65537);
        }
        else if (spacePressed && !isInAir) {
            driver::WriteMemory(driver, client + jump, 256);
        }
        else if (!spacePressed && forceJump == 65537) {
            driver::WriteMemory(driver, client + jump, 256);
        }
    }
}