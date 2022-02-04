#pragma once
#include "../Helpers/Json.hpp"

enum Results
{
	RESULT_OK = 0,
	RESULT_FAIL = 1
};

enum MSG_TYPE
{
	MSG_HELLO = 0,
	MSG_REGISTER = 1,
	MSG_LOGIN = 2,
	MSG_PUSHLIST = 3,
	MSG_SAVELIST = 4
};

class C_Message
{
public:
	C_Message( Results iResult = RESULT_OK, MSG_TYPE iType = MSG_HELLO, std::string strText = "" )
	{
		jMessage[ "Data" ][ "Status" ] = std::to_string( iResult );
		jMessage[ "Data" ][ "Type" ] = std::to_string( iType ).c_str( );
		jMessage[ "Data" ][ "Text" ] = strText;
	}
	
	C_Message( Results iResult, std::string strText )
	{
		jMessage[ "Data" ][ "Status" ] = std::to_string( iResult );
		jMessage[ "Data" ][ "Type" ] = "0";
		jMessage[ "Data" ][ "Text" ] = strText;
	}

	nlohmann::json& GetJSON( )
	{
		return jMessage;
	}

	std::string GetMsgBlank( std::string cBlank )
	{
		return jMessage[ "Data" ][ cBlank ].get< std::string >( );
	}

	nlohmann::json& GetMsgData( std::string cBlank )
	{
		return jMessage[ "Data" ][ cBlank ];
	}

	bool Accept( char* cData )
	{
		if ( !jMessage.accept( cData ) )
			return false;

		jMessage = jMessage.parse( cData );
		return true;
	} 

	const char* DumpMsg( )
	{
		return jMessage.dump( 4 ).c_str( );
	}

private:
	nlohmann::json jMessage;
};