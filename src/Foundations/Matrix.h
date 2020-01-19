/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:56
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:56
 * @Description: All rights reserved
 */
#pragma once
#include <math.h>
#include <string.h>
#include <iostream>
#include "Vector.h"


class Matrix33
{
public:
	double m[ 3 ][ 3 ];

	Matrix33 ()
	{
		m[ 0 ][ 0 ] = m[ 1 ][ 1 ] = m[ 2 ][ 2 ] = 1.f;
		m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = m[ 1 ][ 0 ] =
			m[ 1 ][ 2 ] = m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = 0.f;
	}

	~Matrix33 ()
	{
		//		free ( m );
	}

	Matrix33 ( double mat[ 3 ][ 3 ] )
	{
		m[ 0 ][ 0 ] = mat[ 0 ][ 0 ];
		m[ 0 ][ 1 ] = mat[ 0 ][ 1 ];
		m[ 0 ][ 2 ] = mat[ 0 ][ 2 ];
		m[ 1 ][ 0 ] = mat[ 1 ][ 0 ];
		m[ 1 ][ 1 ] = mat[ 1 ][ 1 ];
		m[ 1 ][ 2 ] = mat[ 1 ][ 2 ];
		m[ 2 ][ 0 ] = mat[ 2 ][ 0 ];
		m[ 2 ][ 1 ] = mat[ 2 ][ 1 ];
		m[ 2 ][ 2 ] = mat[ 2 ][ 2 ];
	}

	Matrix33 ( double t00, double t01, double t02,
	           double t10, double t11, double t12,
	           double t20, double t21, double t22 )
	{
		m[ 0 ][ 0 ] = t00;
		m[ 0 ][ 1 ] = t01;
		m[ 0 ][ 2 ] = t02;
		m[ 1 ][ 0 ] = t10;
		m[ 1 ][ 1 ] = t11;
		m[ 1 ][ 2 ] = t12;
		m[ 2 ][ 0 ] = t20;
		m[ 2 ][ 1 ] = t21;
		m[ 2 ][ 2 ] = t22;
	}

	void initWithVector ( const Vector * a, const Vector * b, const Vector * c )
	{
		m[ 0 ][ 0 ] = a->x;
		m[ 0 ][ 1 ] = b->x;
		m[ 0 ][ 2 ] = c->x;
		m[ 1 ][ 0 ] = a->y;
		m[ 1 ][ 1 ] = b->y;
		m[ 1 ][ 2 ] = c->y;
		m[ 2 ][ 0 ] = a->z;
		m[ 2 ][ 1 ] = b->z;
		m[ 2 ][ 2 ] = c->z;
	}

	void initWithTranVector ( const Vector * a, const Vector * b, const Vector * c )
	{
		m[ 0 ][ 0 ] = a->x;
		m[ 0 ][ 1 ] = a->y;
		m[ 0 ][ 2 ] = a->z;
		m[ 1 ][ 0 ] = b->x;
		m[ 1 ][ 1 ] = b->y;
		m[ 1 ][ 2 ] = b->z;
		m[ 2 ][ 0 ] = c->x;
		m[ 2 ][ 1 ] = c->y;
		m[ 2 ][ 2 ] = c->z;
	}

	Vector multiply ( const Vector * v ) const
	{
		Vector t( 0, 0, 0 );

		t.x = m[ 0 ][ 0 ] * ( v->x ) + m[ 0 ][ 1 ] * ( v->y ) + m[ 0 ][ 2 ] * ( v->z );
		t.y = m[ 1 ][ 0 ] * ( v->x ) + m[ 1 ][ 1 ] * ( v->y ) + m[ 1 ][ 2 ] * ( v->z );
		t.z = m[ 2 ][ 0 ] * ( v->x ) + m[ 2 ][ 1 ] * ( v->y ) + m[ 2 ][ 2 ] * ( v->z );

		return t;
	}

	Matrix33 Inverse33 () const
	{
		int indxc[ 3 ], indxr[ 3 ];
		int ipiv[ 3 ] = { 0, 0, 0 };
		double minv[ 3 ][ 3 ];
		memcpy( minv, m, 3 * 3 * sizeof ( double) );
		for ( int i = 0; i < 3; i++ )
		{
			int irow = -1, icol = -1;
			double big = 0.;
			// Choose pivot
			for ( int j = 0; j < 3; j++ )
			{
				if ( ipiv[ j ] != 1 )
				{
					for ( int k = 0; k < 3; k++ )
					{
						if ( ipiv[ k ] == 0 )
						{
							if ( abs( minv[ j ][ k ] ) >= big )
							{
								big = double( abs( minv[ j ][ k ] ) );
								irow = j;
								icol = k;
							}
						}
						else if ( ipiv[ k ] > 1 )
							printf( "Singular matrix in MatrixInvert" );
					}
				}
			}
			++ipiv[ icol ];
			// Swap rows _irow_ and _icol_ for pivot
			if ( irow != icol )
			{
				for ( int k = 0; k < 3; ++k )
					Swap( minv[ irow ][ k ], minv[ icol ][ k ] );
			}
			indxr[ i ] = irow;
			indxc[ i ] = icol;
			if ( minv[ icol ][ icol ] == 0. )
				printf( "Singular matrix in MatrixInvert" );

			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			double pivinv = 1.f / minv[ icol ][ icol ];
			minv[ icol ][ icol ] = 1.f;
			for ( int j = 0; j < 3; j++ )
				minv[ icol ][ j ] *= pivinv;

			// Subtract this row from others to zero out their columns
			for ( int j = 0; j < 3; j++ )
			{
				if ( j != icol )
				{
					double save = minv[ j ][ icol ];
					minv[ j ][ icol ] = 0;
					for ( int k = 0; k < 3; k++ )
						minv[ j ][ k ] -= minv[ icol ][ k ] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for ( int j = 3; j >= 0; j-- )
		{
			if ( indxr[ j ] != indxc[ j ] )
			{
				for ( int k = 0; k < 3; k++ )
					Swap( minv[ k ][ indxr[ j ] ], minv[ k ][ indxc[ j ] ] );
			}
		}
		return Matrix33( minv );
	}
};

