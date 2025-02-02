/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:20:07
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:20:07
 * @Description: All rights reserved
 */
#include <iostream>
#include <Core\LightMapGen.h>
#include <Core/SceneFileParser.h>
#include <time.h>
#include <stdio.h>
using namespace std;

int main ( int argc , char * argv [] )
{

	double start , end , cost;
	start = clock ();

	char inputSceneFile[ 50 ];

	if (argv[ 1 ] == nullptr)
	{
		printf ( "No input file" );
		return 0;
	}

	strcpy_s ( inputSceneFile , argv[ 1 ] );

	SceneFileParser sfp;
	LightMapGen	lightMapGenerator;

	// parse input file
	sfp.ParseSceneFile ( inputSceneFile , lightMapGenerator.myScene );

	
	lightMapGenerator.InitialiseGenerator (&sfp);

	lightMapGenerator.Run ();

	end = clock ();
	cost = end - start;
	printf ( "Time cost : %f s \n" , cost / CLK_TCK );

	return 0;
}
