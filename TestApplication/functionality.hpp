#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

#include "memory.hpp"
#include "buttons.hpp"
#include "offsets.hpp"
#include "client.dll.hpp"

using namespace std;
using namespace cs2_dumper::buttons;
using namespace cs2_dumper::offsets;
using namespace cs2_dumper::schemas::client_dll;

bool ToggleBhop(bool toggle, HANDLE driver, uintptr_t client);
