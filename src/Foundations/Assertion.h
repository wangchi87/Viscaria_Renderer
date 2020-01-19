/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:39
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:39
 * @Description: All rights reserved
 */
#include <crtdbg.h>
#include <float.h>
#pragma once

//#define ASSERTION_ON


inline void ASSERT_NAN ( double num )
{
#ifdef ASSERTION_ON

	if (_isnan ( num ))
		printf ( "nan" );

	_ASSERT ( !_isnan ( num ) );

#endif
}

