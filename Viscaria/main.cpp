/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:20:28
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:20:28
 * @Description: All rights reserved
 */
#include <iostream>
#include <Core/RenderRunner.h>
#include <Core/SceneFileParser.h>
#include <time.h>
#include <stdio.h>
using namespace std;

int main ( int argc , char * argv [] )
{
	cout << "This is a simple 3D renderer developed by Chi Wang." << endl;
	cout << "I started this project in my spare time since June of 2016, during my stay in Prague." << endl;
	cout << "Many thanks to my supervisor Alexander Wilkie, CGG of Charles University and Chinese Scholarship Council. ";
	cout << "I would never be able to work on my own renderer without their supports." << endl;
	

	double start , end , cost;
	start = clock ( );

	char inputSceneFile[ 50 ];

	if ( argv[ 1 ] == nullptr )
	{
		printf ( "No input file" );
		return 0;
	}

	strcpy_s ( inputSceneFile , argv[ 1 ] );

	SceneFileParser sfp;
	RenderRunner	rr;

	// parse input file
	sfp.ParseSceneFile ( inputSceneFile , rr.myScene );

	rr.InitialiseRenderer ( &sfp );

	// rendering
	rr.Run ();

	end = clock ( );
	cost = end - start;
	printf ( "Time cost : %f s \n" , cost/ CLK_TCK );

	return 0;
}
