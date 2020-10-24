#pragma once

#pragma warning(disable : 26812)

#include <Windows.h>
#include <synchapi.h>
#include <thread>
#include <ntstatus.h>
#include <time.h>
#include <random>

#include "Requests.hpp"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef struct _CALL_DATA
{
	PVOID Function;
	ULONG_PTR Param;
} CALL_DATA, * PCALL_DATA;

void CallFunction(PCALL_DATA CallData)
{
	using tFn = NTSTATUS(__stdcall*)(UINT_PTR Param1);
	const auto fn = static_cast<tFn>(CallData->Function);

	fn(CallData->Param);
}

extern "C" std::uint64_t __stdcall SysQueryCompositionSurfaceStatistics( std::uint32_t, void* command );

#ifndef forceinline
#define forceinline __forceinline
#endif

#define pog_secret 0x5438679

namespace shared
{
	enum command_type : std::uint32_t
	{
		verify_hook = 0,

		read_memory = 1,
		write_memory = 2,
		write_read_memory = 3,

		base_address = 4,

		unregister_callback = 5,
		register_callback = 6,
		unhook_handler = 7
	};
	struct command_t
	{
		command_type type;

		union
		{
			struct
			{
				std::uint32_t process_id;
				std::uintptr_t address;
				std::uintptr_t buffer;
				std::uint32_t size;
			} copy_memory;
			struct
			{
				std::uint32_t process_id;
			} base_address;
		};
	};
}

class pogpass_cxt_c
{
public:

	forceinline pogpass_cxt_c( )
	{

	}
	forceinline ~pogpass_cxt_c( )
	{

	}

	forceinline auto is_hooked( ) const -> bool
	{
		return this->call_hook( shared::verify_hook ) == 0x1337;
	}
	forceinline auto unhook_handler( ) const -> bool
	{
		return this->call_hook( shared::unhook_handler ) == 0x1337;
	}

	template <bool state> forceinline auto set_callback( ) const -> bool
	{
		return this->call_hook( state ? shared::register_callback : shared::unregister_callback ) == 0x1337;
	}

	auto main_module( std::uint32_t process_id ) -> std::uint64_t
	{
		auto command = shared::command_t{ shared::base_address };
		command.base_address.process_id = process_id;

		return this->call_hook( &command );
	}

	auto virtual_read( std::uint32_t process_id, std::uintptr_t address, std::uintptr_t buffer, std::uint32_t size ) -> std::uint32_t
	{
		auto command = shared::command_t{ shared::read_memory };
		command.copy_memory.process_id = process_id;
		command.copy_memory.address = address;
		command.copy_memory.buffer = buffer;
		command.copy_memory.size = size;

		return this->call_hook( &command );
	}
	template <bool write_read> auto virtual_write( std::uint32_t process_id, std::uintptr_t address, std::uintptr_t buffer, std::uint32_t size ) -> std::uint32_t
	{
		auto command = shared::command_t{ write_read ? shared::write_read_memory : shared::write_memory };
		command.copy_memory.process_id = process_id;
		command.copy_memory.address = address;
		command.copy_memory.buffer = buffer;
		command.copy_memory.size = size;

		return this->call_hook( &command );
	}

private:

	forceinline auto call_hook( shared::command_t* send_command ) const -> std::uint64_t
	{
		return SysQueryCompositionSurfaceStatistics( pog_secret, send_command );
	}
	forceinline auto call_hook( shared::command_type type ) const -> std::uint64_t
	{
		auto send_command = shared::command_t{ type };
		return SysQueryCompositionSurfaceStatistics( pog_secret, &send_command );
	}

};

class KernelInterface
{
public:

	// No inheritance because dat ugli and conflicting
	// styles >:(
	pogpass_cxt_c* k_cxt = nullptr;

public:

	KernelInterface( )
	{
		this->k_cxt = new pogpass_cxt_c( );
	}
	~KernelInterface( )
	{
		delete this->k_cxt;
	}

	HANDLE TargetProcessId;

	NTSTATUS Setup()
	{
		if ( !LoadLibraryA( "user32.dll" ) || !LoadLibraryA( "win32u.dll" ) )
		{
			std::printf( "[-] Failed to load required libraries\n" );
			return STATUS_NOT_FOUND;
		}

		return k_cxt->is_hooked() ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
	}

	NTSTATUS Unload()
	{
		return this->k_cxt->unhook_handler( ) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
	}

	void SetTargetProcessId(HANDLE ProcessId)
	{
		this->TargetProcessId = ProcessId;
	}

	UINT_PTR GetMainModule()
	{
		return this->k_cxt->main_module( reinterpret_cast< std::uint32_t >( this->TargetProcessId ) );
	}

	NTSTATUS Read(PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
	{
		return this->k_cxt->virtual_read
		(
			reinterpret_cast< std::uint32_t >( this->TargetProcessId ),
			reinterpret_cast< std::uint64_t >( SourceAddress ),
			reinterpret_cast< std::uint64_t >( TargetAddress ),
			Size
		) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
	}

	template<typename T>
	T Read(UINT_PTR Address)
	{
		T Buffer = { };

		if (!NT_SUCCESS(Read((PVOID)Address, &Buffer, sizeof(T))))
		{
			RtlZeroMemory(&Buffer, sizeof(Buffer));
		}

		return Buffer;
	}

	NTSTATUS Write(PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
	{
		return this->k_cxt->virtual_write< false >
		(
			reinterpret_cast< std::uint32_t >( this->TargetProcessId ),
			reinterpret_cast< std::uint64_t >( TargetAddress ),
			reinterpret_cast< std::uint64_t >( SourceAddress ),
			Size
		) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
	}

	template<typename T>
	NTSTATUS Write(UINT_PTR Address, T Buffer)
	{
		return Write(&Buffer, (PVOID)Address, sizeof(T));
	}

	NTSTATUS WriteToReadOnly(PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
	{
		return this->k_cxt->virtual_write< true >
		(
				reinterpret_cast< std::uint32_t >( this->TargetProcessId ),
				reinterpret_cast< std::uint64_t >( TargetAddress ),
				reinterpret_cast< std::uint64_t >( SourceAddress ),
				Size
		) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
	}

	template<typename T>
	NTSTATUS WriteToReadOnly(UINT_PTR Address, T Buffer)
	{
		return WriteToReadOnly(&Buffer, (PVOID)Address, sizeof(T));
	}

};