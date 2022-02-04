#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Time.hpp"
#include <chrono>

inline std::string Date( )
{
	int32_t iSeconds = std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
	return std::to_string( iSeconds );
}

inline void PushLog( const char* data )
{
	static bool bOnce = false;

	static std::string fileName = "../Logs/";
	if ( !bOnce )
	{
		fileName += Date( );
		fileName += ".log";
		
		bOnce = true;
	}

	std::ofstream strLogFile = std::ofstream( fileName, std::ios::app );
	strLogFile << data;
	strLogFile.close( );
}

static bool g_bOnce = false;
static bool g_bShouldOnce = false;

inline void LogData( const char* data )
{
	if ( g_bOnce )
		printf( "\n" );

	std::string strMsg = "[" + TIME + "][ Server ] " + data;
	printf( strMsg.c_str( ) );

	std::string strLogMsg = "\n";
	if ( !g_bOnce )
		strLogMsg = "";
	strLogMsg += "[" + TIME + "] ";
	strLogMsg += data;

	PushLog( strLogMsg.c_str( ) );

	if ( !g_bShouldOnce )
	{
		g_bShouldOnce = true;
		g_bOnce = true;
	}
}

#define LOG( s ) LogData( s )