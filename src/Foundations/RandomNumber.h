/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:05
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:05
 * @Description: All rights reserved
 */
#pragma once
#include <random>

#define NON_DETERMINISTIC_RANDOM_NUMBER true

static std::random_device rd; // non-deterministic generator
static std::mt19937 gen( rd() );

inline double getUniformRandomNumber ( double min, double max )
{
	if ( NON_DETERMINISTIC_RANDOM_NUMBER == true )
	{
		std::uniform_real_distribution<> distr( min, max );

		return distr( gen );
	}
	else
	{
		// this is a deterministic random number generator
		double r = ( double ) ( rand() - 1 ) * ( max - min ) / RAND_MAX + min;
		return r;
	}
}

