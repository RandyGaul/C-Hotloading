/*
	Copyright (c) 2016 Randy Gaul http://www.randygaul.net

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	  1. The origin of this software must not be misrepresented; you must not
		 claim that you wrote the original software. If you use this software
		 in a product, an acknowledgment in the product documentation would be
		 appreciated but is not required.
	  2. Altered source versions must be plainly marked as such, and must not
		 be misrepresented as being the original software.
	  3. This notice may not be removed or altered from any source distribution.
*/

#include <Windows.h>
#include <cstdio>
#include "game.h"

typedef void (*LoopType)( );
LoopType LoopPtr;
HMODULE GameDLL;
FILETIME GameDLLWriteTime;

FILETIME Win32GetLastWriteTime( char* path )
{
	FILETIME time = {};
	WIN32_FILE_ATTRIBUTE_DATA data;

	if ( GetFileAttributesEx( path, GetFileExInfoStandard, &data ) )
		time = data.ftLastWriteTime;

	return time;
}

void UnloadGameDLL( )
{
	FreeLibrary( GameDLL );
	GameDLL = 0;
	LoopPtr = 0;
}

void LoadGameDLL( )
{
	WIN32_FILE_ATTRIBUTE_DATA unused;
	if ( !GetFileAttributesEx( "lock.tmp", GetFileExInfoStandard, &unused ) )
	{
		UnloadGameDLL( );
		CopyFile( "game.dll", "game_temp.dll", 0 );
		GameDLL = LoadLibrary( "game_temp.dll" );

		if ( !GameDLL )
		{
			DWORD err = GetLastError( );
			printf( "Can't load lib: %d\n", err );
			return;
		}

		LoopPtr = (LoopType)GetProcAddress( GameDLL, "Loop" );
		if ( !LoopPtr )
		{
			DWORD err = GetLastError( );
			printf( "Cant load func: %d\n", err );
			return;
		}

		GameDLLWriteTime = Win32GetLastWriteTime( "game.dll" );
	}
}

int main( )
{
	char buf[ 256 ];
	GetCurrentDirectory( 256, buf );

	LoadGameDLL( );

	while( 1 )
	{
		FILETIME newTime = Win32GetLastWriteTime( "game.dll" );

		if ( CompareFileTime( &newTime, &GameDLLWriteTime ) )
			LoadGameDLL( );

		LoopPtr( );
	}

	return 0;
}
