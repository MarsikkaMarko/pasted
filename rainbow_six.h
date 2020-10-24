#pragma once


uintptr_t game_manager()
{
    return Interface->Read<uintptr_t>(MainModule + 0x5E00F28);
}

uintptr_t profile_manager()
{
    return Interface->Read<uintptr_t>(MainModule + 0x5E316E0);
}

uintptr_t fov_manager()
{
    return Interface->Read<uintptr_t>(MainModule + 0x6D0E3D0);
}

uintptr_t network_manager()
{
    return Interface->Read<uintptr_t>(MainModule + decrypt(offsets::network_manager, custom_keys::network_manager_key));
}

uintptr_t enviroment_manager()
{
    return Interface->Read<uintptr_t>(MainModule + 0x5E06DD0);
}

uintptr_t round_manager()
{
    return Interface->Read<uintptr_t>(MainModule + 0x70A47F8);
}

uintptr_t spoof_spectate_manager()
{
    return Interface->Read<uintptr_t>(MainModule + offsets::spoof_spectate_manager);
}

uintptr_t noclip_manager()
{
    return Interface->Read<uintptr_t>(MainModule + offsets::noclip_manager);
}

int game_state()
{
    return Interface->Read<int>( round_manager( ) + 0x300 );
}

uint64_t get_localplayer()
{
    uint64_t localPlayer = Interface->Read<uint64_t>(profile_manager() + 0x88); //0x5E316E0
    localPlayer = Interface->Read<uint64_t>(localPlayer);
    localPlayer = Interface->Read<uint64_t>(localPlayer + 0x30);
    localPlayer -= 0x77;
    localPlayer = __ROL8__(localPlayer, 0x6);
    localPlayer -= 0x59;

    return localPlayer;
}

uint64_t get_weapon_info()
{
    uint64_t weaponInfo = Interface->Read<uint64_t>(get_localplayer() + 0x90);
    weaponInfo = Interface->Read<uint64_t>(weaponInfo + 0x70);
    weaponInfo = Interface->Read<uint64_t>(weaponInfo + 0x288);
    weaponInfo = __ROL8__(weaponInfo, 0x3E);
    weaponInfo -= 0x68;
    weaponInfo = __ROL8__(weaponInfo, 0x15);

    return weaponInfo;
}

uintptr_t event_manager()
{
    uint64_t eventManager = Interface->Read<uint64_t>(get_localplayer() + 0x30);
    eventManager ^= 0xABD61E6916FBC7F2;
    eventManager += 0x3894F6F1D31B186C;
    eventManager ^= 0xC9956FF9443397DE;

    return eventManager;
}