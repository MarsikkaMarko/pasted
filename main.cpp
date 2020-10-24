#include <iostream>
#include <Windows.h>

#include "includes.h"
#include "menu.h"

// Whyt do u not have this lmao
forceinline auto find_process_id( std::wstring process_name ) -> std::uint32_t
{
    if ( process_name.empty( ) )
        return 0;

    const auto snapshot = std::unique_ptr<void, decltype( &CloseHandle )>{ CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 ), &CloseHandle };

    if ( snapshot.get( ) == INVALID_HANDLE_VALUE )
        return 0;

    PROCESSENTRY32W process_entry{ sizeof( PROCESSENTRY32W ) };

    for ( Process32FirstW( snapshot.get( ), &process_entry ); Process32NextW( snapshot.get( ), &process_entry ); )
        if ( process_name.compare( process_entry.szExeFile ) == 0 )
            return process_entry.th32ProcessID;

    return 0;
}

void feature_loop()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));//
        if (game_state() == 2)
        {
            cav_esp();
            player_fov();
            gun_fov();
            no_spread();
            no_recoil();
            fire_mode();
            no_flash();
            long_knife();
            fullbright();
            spoof_spectate();
            //run_and_shoot();
            speed();
            //glow();
            ads();
        }
        else if (game_state() == 3)
        {
            cav_esp();
            player_fov();
            gun_fov();
            no_spread();
            no_recoil();
            fire_mode();
            no_flash();
            long_knife();
            fullbright();
            spoof_spectate();
            //run_and_shoot();
            noclip();
            speed();
            //glow();
            ads();
        }
        unlock_all();
    }
}

void aim_loop()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_aim));
        if (features::aim_enabled_state == true)
        {
            set_key();
            aimbot();
        }
    }
}

int main(void)
{
    if ( !NT_SUCCESS( Interface->Setup( ) ) )
    {
        std::printf( "[!] Failed to verify hook. Driver loaded?\n" );

        std::cin.get( );
        return -1;
    }

    std::printf( "[+] Driver is loaded\n\n" );

    /*
    
    // You NEEEEEEEEEEED TO DO THIS BEFORE GAME LAUNCH
    // UNLESS U WANT BAN
    if ( !Interface->k_cxt->set_callback<true>( ) )
    {
        std::printf( "[-] Failed to set image load callback\n" );

        std::cin.get( );
        return 0;
    }

    // Load game quickly to avoid getting ass fucked by pg
    std::printf( "[=] Please load the game QUICKLY to avoid a BSOD\n" );

    while ( !FindWindowA( "R6Game", nullptr ) ) { Sleep( 10 ); }

    std::printf( "[+] Found game!\n" );

    // YOU NEEEEEED TO DO THIS AFTER GAME LAUNCH IF YOU DONT
    // WANNA BSOD
    if ( !Interface->k_cxt->set_callback<false>( ) )
    {
        std::printf( "[-] Failed to remove image load callback.. BSOD time lmao\n" );

        std::cin.get( );
        return 0;
    }
    */

    const auto gprocid = find_process_id( L"RainbowSix.exe" );

    Interface->SetTargetProcessId( reinterpret_cast< HANDLE >( gprocid ) );
    MainModule = Interface->GetMainModule( );
    const auto dos = Interface->Read<short>( MainModule );

    std::printf( "[+] Success, happy playing! 0x%x : 0x%llx : 0x%x\n", gprocid, MainModule, dos );
     
    std::thread( draw_menu ).detach( );
    std::thread( feature_loop ).detach( );
    std::thread( aim_loop ).detach( );

    while ( 1 ) { Sleep( 10000 ); }
}