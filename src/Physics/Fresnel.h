/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:45
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:45
 * @Description: All rights reserved
 */

#pragma once
#ifndef Fresnel_h
#define Fresnel_h

#define MATH_RAD_TO_DEG 180/M_PI
#define MATH_DEG_TO_RAD M_PI/180

#endif /* Fresnel_h */

#include "Foundations/Vector.h"


inline void GetSpecularRefractionDirection (
	const Vector incidentDirection,
	const Vector normalDirection,
	const double n_i,
	const double n_t,
	bool & TIR,
	Vector & refractionDirection
)
{
	double eta = n_i / n_t;
	// Make sure the local Normal  is in the same side of incidence
	Vector localNormal = normalDirection;

	//Propagate Downwards
	double cosi = Dot( incidentDirection,
	                   normalDirection );
	if ( cosi > 0 )
		localNormal = - ( normalDirection ) ;

	double idotN = fabs( cosi );

	double k = 1 - eta * eta * ( 1 - idotN * idotN ); //cos_t * cos_t

	if ( k < 0 )
	{
		// total internal reflection;
		TIR = true;
		//		NSLog(@"TIR");
	}
	else
	{
		TIR = false;
		Vector refraction;

		Vector temp1;
		temp1 = eta * ( incidentDirection );
		Vector temp2;
		temp2 = ( eta * idotN - sqrt( k ) ) * ( localNormal );

		refraction = temp1 + temp2 ;

		refractionDirection = Normalize( refraction );
	}


	//^^^^^^^ test ^^^^^^^
	/*
	double cost = vec3d_vv_dot( refractionDirection ,
	                            normalDirection
	                          );
	double sint = sqrt(1-cost*cost);
	double sini = sqrt(1-cosi*cosi);
	NSLog(@" theta_i %f   theta_t  %f",asin(sini),asin(sint));
	NSLog(@" NiSini %f   NtSint  %f",n_i*sini,n_t*sint);
	*/

	//vec3d_printf(incidentDirection);
	//vec3d_printf(refractionDirection);
}

inline void GetSpecularReflectionDirection (
	const Vector incidentDirection,
	const Vector normalDirection,
	Vector & reflectionDirection
)
{
	Vector localNormal = normalDirection;

	//Propagate Downwards
	double cosi = Dot( incidentDirection,
	                   normalDirection );
	if ( cosi > 0 )
		localNormal = - ( normalDirection ) ;

	reflectionDirection = Normalize(
		incidentDirection
		+
		2 * ( AbsDot( incidentDirection, localNormal ) * localNormal )
	);


	//	double dot_i = Dot ( normalDirection , incidentDirection );
	//	double dot_o = Dot ( normalDirection , reflectionDirection );
	//
	//	if (dot_i*dot_o > 0)
	//		printf ( "Error!" );
}


// @ theta	: angle between incidence and half vector (in RAD)
// @ n1 k1	: refraction index of material 
// @ n2 k2	: refraction index of material 
// @ phase  : phase shift of refraction
// @ Ts Tp  : transmittance of s and p component
// return   : transmittance
inline double Fresnel_Refraction (
	double theta,
	double n1,
	double k1,
	double n2,
	double k2,
	double & phase,
	double & Ts,
	double & Tp,
	double & TsTp
)
{
	double nab = n1 / n2;
	double theta_t = asin( n1 * sin( theta ) / n2 );

	Tp = ( 2 * nab * cos( theta ) / ( cos( theta ) + nab * cos( theta_t ) ) ) *
		( 2 * nab * cos( theta ) / ( cos( theta ) + nab * cos( theta_t ) ) );
	Ts = ( 2 * nab * cos( theta ) / ( nab * cos( theta ) + cos( theta_t ) ) ) *
		( 2 * nab * cos( theta ) / ( nab * cos( theta ) + cos( theta_t ) ) );

	// phas shift for refraction is zero
	phase = 0;

	TsTp = ( 4 * nab * nab * cos( theta ) * cos( theta ) ) /
		( ( cos( theta ) + nab * cos( theta_t ) ) * ( nab * cos( theta ) + cos( theta_t ) ) );

	double nba = n2 / n1;

	double trans_factor = /* nba * nba * */ nba * ( cos( theta_t ) / cos( theta ) );

	return trans_factor * ( Ts + Tp ) / 2;
}

// @ theta	: angle between incidence and half vector (in RAD)
// @ n1 k1	: refraction index of material 
// @ n2 k2	: refraction index of material 
// @ rs rp  : amptitude of reflection s and p component
// @ phaseS phaseP : phase shift of s and p component
// @ Fs Fp  : reflectance of s and p component
// return	: reflectance
inline double Fresnel_Reflection (
	double theta,
	double n1,
	double k1,
	double n2,
	double k2,
	double & rs,
	double & rp,
	double & phaseS,
	double & phaseP,
	double & Fs,
	double & Fp )
{
	double temp = ( n2 * n2 - k2 * k2 - n1 * n1 * sin( theta ) * sin( theta ) );
	double p2 = ( sqrt( temp * temp + 4 * n2 * n2 * k2 * k2 ) + temp ) / 2;
	double q2 = ( sqrt( temp * temp + 4 * n2 * n2 * k2 * k2 ) - temp ) / 2;
	double p = sqrt( p2 );
	double q = sqrt( q2 );

	double brewsterAngle = atan( n2 / n1 );

	Fs = ( ( n1 * cos( theta ) - p ) * ( n1 * cos( theta ) - p ) + q2 ) /
		( ( n1 * cos( theta ) + p ) * ( n1 * cos( theta ) + p ) + q2 );
	Fp = ( Fs ) * ( ( p - n1 * sin( theta ) * tan( theta ) ) *
			( p - n1 * sin( theta ) * tan( theta ) ) + q2 )
		/ ( ( p + n1 * sin( theta ) * tan( theta ) )
			* ( p + n1 * sin( theta ) * tan( theta ) ) + q2 );

	// phase shift for dielectric materials
	// please refer to section 8.2 of < Polarised Light, second edition >,  Dennis Goldstein
	if ( n1 > n2 && k2 == 0 )
	{
		double criticalAngle = asin( n2 / n1 );

		if ( theta < criticalAngle )
			phaseS = 0 ;
		else
			phaseS = 2 * atan( sqrt( sin( theta ) * sin( theta )
					- sin( criticalAngle ) * sin( criticalAngle )
				)
				/ cos( criticalAngle )
			);


		if ( theta < brewsterAngle )
			phaseP = 180 * MATH_DEG_TO_RAD;
		else if ( theta < criticalAngle )
			phaseP = 0 * MATH_DEG_TO_RAD;
		else
			phaseP = 2 * atan( sqrt( sin( theta ) * sin( theta )
					- sin( criticalAngle ) * sin( criticalAngle )
				)
				/ ( cos( criticalAngle ) * sin( criticalAngle ) * sin( criticalAngle )
				)
			);
	}

	if ( n1 < n2 && k2 == 0 )
	{
		phaseS = 180 * MATH_DEG_TO_RAD;
		phaseP = ( theta < brewsterAngle ) ? 0
			: 180 * MATH_DEG_TO_RAD;
	}

	// phase shift for conductor
	if ( k2 != 0 )
	{
		phaseS = atan( 2 * n1 * q * cos( theta ) / ( n1 * n1 * cos( theta ) * cos( theta ) - p2 - q2 ) );
		phaseP = atan(
			(
				-2 * n1 * q * cos( theta )
				* ( p2 + q2 - n1 * n1 * sin( theta ) * sin( theta ) ) )
			/ (
				( n2 * n2 + k2 * k2 ) * ( n2 * n2 + k2 * k2 ) * cos( theta ) * cos( theta ) - n1 * n1 * ( p2 + q2 )
			)
		);
	}


	rs = sqrt( fabs( Fs ) );
	rp = sqrt( fabs( Fp ) );

	return ( Fs + Fp ) / 2;
}


inline double Fresnel_Refraction_plain ( double theta, double n1, double k1, double n2, double k2 )
{
	double nab = n1 / n2;
	double theta_t = asin( n1 * sin( theta ) / n2 );

	double Ts, Tp, factor;

	Tp = ( 2 * nab * cos( theta ) / ( cos( theta ) + nab * cos( theta_t ) ) ) *
		( 2 * nab * cos( theta ) / ( cos( theta ) + nab * cos( theta_t ) ) );
	Ts = ( 2 * nab * cos( theta ) / ( nab * cos( theta ) + cos( theta_t ) ) ) *
		( 2 * nab * cos( theta ) / ( nab * cos( theta ) + cos( theta_t ) ) );


	double nba = n2 / n1;
	factor = /* nba * nba * */ nba * ( cos( theta_t ) / cos( theta ) );

	return factor * ( Ts + Tp ) / 2;
}

inline double Fresnel_Reflection_Plain ( double theta, double n1, double k1, double n2, double k2 )
{
	double temp = ( n2 * n2 - k2 * k2 - n1 * n1 * sin( theta ) * sin( theta ) );
	double p2 = ( sqrt( temp * temp + 4 * n2 * n2 * k2 * k2 ) + temp ) / 2;
	double q2 = ( sqrt( temp * temp + 4 * n2 * n2 * k2 * k2 ) - temp ) / 2;
	double p = sqrt( p2 );


	double Fs;
	double Fp;

	Fs = ( ( n1 * cos( theta ) - p ) * ( n1 * cos( theta ) - p ) + q2 ) /
		( ( n1 * cos( theta ) + p ) * ( n1 * cos( theta ) + p ) + q2 );
	Fp = ( Fs ) * ( ( p - n1 * sin( theta ) * tan( theta ) ) *
			( p - n1 * sin( theta ) * tan( theta ) ) + q2 )
		/ ( ( p + n1 * sin( theta ) * tan( theta ) )
			* ( p + n1 * sin( theta ) * tan( theta ) ) + q2 );

	return ( Fs + Fp ) / 2;
}


inline void rotationAngle (
	const Vector incomingDirection,
	const Vector outgoingDirection,
	const Vector surfaceNormal,
	const Vector localNormal,
	double & globalTolocalAngle,
	double & localToGlobalAngle
)
{
	Vector Si, Sr, Ei, Er, temp_in, temp_out;
	Si = Cross( incomingDirection, surfaceNormal );
	Si = Normalize( Si );
	Sr = -Cross( outgoingDirection, surfaceNormal );
	Sr = Normalize( Sr );
	Ei = Cross( incomingDirection, localNormal );
	Ei = Normalize( Ei );
	Er = -Cross( outgoingDirection, localNormal );
	Er = Normalize( Er );
	double cos_rotate_in, cos_rotate_out, sin_rotate_in, sin_rotate_out;
	cos_rotate_in = Clamp( CosBetweenVector( Si, Ei ), -1, 1 );
	cos_rotate_out = Clamp( CosBetweenVector( Sr, Er ), -1, 1 );
	sin_rotate_in = sqrt( 1 - cos_rotate_in * cos_rotate_in );
	sin_rotate_out = sqrt( 1 - cos_rotate_out * cos_rotate_out );

	//Rotation angle for incident light and refraction light
	globalTolocalAngle = acos( cos_rotate_in );
	localToGlobalAngle = acos( cos_rotate_out );

	temp_in = Cross( Si, Ei );
	temp_out = Cross( Er, Sr );

	//Determine rotating a positive or negtive angle for incident and reflection light
	if ( Dot( temp_in, incomingDirection ) < 0 )
	{
		globalTolocalAngle = 2 * M_PI - ( globalTolocalAngle );
	}

	if ( Dot( temp_out, outgoingDirection ) < 0 )
	{
		localToGlobalAngle = 2 * M_PI - ( localToGlobalAngle );
	}
}

inline bool isNormalReflection (
	const Vector incomingDirection,
	const Vector outgoingDirection
)
{
	double zofin = incomingDirection.z;
	double zofout = outgoingDirection.z;

	if ( zofin * zofout < 0 )
		return true;
	else return false;
}

inline bool isNormalRefraction (
	const Vector incomingDirection,
	const Vector outgoingDirection
)
{
	double zofin = incomingDirection.z;
	double zofout = outgoingDirection.z;

	if ( zofin * zofout > 0 )
		return true;
	else return false;
}

