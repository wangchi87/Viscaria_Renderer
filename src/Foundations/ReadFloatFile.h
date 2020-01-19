/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:09
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:09
 * @Description: All rights reserved
 */
#pragma once

#include <ctype.h>
#include <stdlib.h>

#include <vector>
using std::vector;


bool inline ReadFloatFile ( const char * filename, vector<float> * values )
{
	errno_t err;
	FILE * f;
	err = fopen_s( &f, filename, "r" );
	if ( err != 0 )
	{
		printf( "Unable to open file \"%s\"", filename );
		return false;
	}

	int c;
	bool inNumber = false;
	char curNumber[ 32 ];
	int curNumberPos = 0;
	int lineNumber = 1;
	while ( ( c = getc( f ) ) != EOF )
	{
		if ( c == '\n' ) ++lineNumber;
		if ( inNumber )
		{
			if ( isdigit( c ) || c == '.' || c == 'e' || c == '-' || c == '+' )
				curNumber[ curNumberPos++ ] = c;
			else
			{
				curNumber[ curNumberPos++ ] = '\0';
				values->push_back( atof( curNumber ) );
				//				assert ( curNumberPos < ( int )sizeof ( curNumber ) );
				inNumber = false;
				curNumberPos = 0;
			}
		}
		else
		{
			if ( isdigit( c ) || c == '.' || c == '-' || c == '+' )
			{
				inNumber = true;
				curNumber[ curNumberPos++ ] = c;
			}
			else if ( c == '#' )
			{
				while ( ( c = getc( f ) ) != '\n' && c != EOF );
				++lineNumber;
			}
			else if ( !isspace( c ) )
			{
				printf( "Unexpected text found at line %d of float file \"%s\"",
				        lineNumber, filename );
			}
		}
	}
	fclose( f );
	return true;
}

