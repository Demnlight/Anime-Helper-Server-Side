#pragma once
#include <time.h>
#include <string>

#pragma warning(disable : 4996)

inline std::string GetCurrentDataTime( )
{
    time_t     now = time( 0 );
    struct tm  tstruct;
    char       buf[ 80 ];
    tstruct = *localtime( &now );
    strftime( buf, sizeof( buf ), "%X", &tstruct );

    return buf;
}

#define TIME GetCurrentDataTime( )