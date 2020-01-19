#include "MicroSurface.h"


MicroSurface::MicroSurface ()
{
	alpha_x = 0.1;
	alpha_y = 0.1;
}


MicroSurface::~MicroSurface ()
{
}

double Beckmann::D_wh (
	const Vector * wh_local,
	const Vector * wh_global )
{
	double dot_mn = Dot( *wh_local, *wh_global );
	if ( X_plus( dot_mn ) == 1 )
	{
		double theta_m = Clamp( acos( dot_mn ), 0, M_PI );

		if ( std::isinf( tan( theta_m ) ) )
			return 0;

		return exp( -pow( ( tan( theta_m ) / alpha_x ), 2 ) )
			/ ( M_PI * pow( alpha_x, 2 ) * pow( cos( theta_m ), 4 ) );
	}
	return 0;
}

Vector Beckmann::sample_wh ()
{
	double r1, r2;
	r1 = getUniformRandomNumber( 0, 1 );
	r2 = getUniformRandomNumber( 0, 1 );

	// walter's trick
	//		double alpha_trick = (1.2-0.2*sqrt( in_coming, wh_local ))

	double theta = atan( sqrt( -alpha_x * alpha_x * log( 1 - r1 ) ) );
	double phi = M_PI * 2 * r2;

	ASSERT_NAN( theta );
	ASSERT_NAN( phi );

	return SphericalDirection( theta, phi );
}

/**
 * \brief sample normal direction on Beckmann micro-surface
 * \param wi_global 
 * \param wh_global 
 * \return 
 */
Vector Beckmann::sample_wh (
	const Vector & wi_global,
	const Vector & wh_global )
{
	double r1, r2;
	r1 = getUniformRandomNumber( 0, 1 );
	r2 = getUniformRandomNumber( 0, 1 );

	//	 walter's trick
	double alpha_trick = ( 1.2 - 0.2 * sqrt( AbsDot( wi_global, wh_global ) ) * alpha_x );

	double theta = atan( sqrt( -alpha_x * alpha_x * log( 1 - r1 ) ) );
	double phi = M_PI * 2 * r2;

	ASSERT_NAN( theta );
	ASSERT_NAN( phi );

	return SphericalDirection( theta, phi );
}

/**
 * \brief masking function of Beckmann surface
 * \param w 
 * \param wh_local 
 * \param wh_global 
 * \return 
 */
double Beckmann::masking (
	const Vector * w,
	const Vector * wh_local,
	const Vector * wh_global )
{
	double dot_wm = Dot( *w, *wh_local );
	double dot_wn = Dot( *w, *wh_global );

	if ( X_plus( dot_wm / dot_wn ) == 1 )
	{
		double theta_v = Clamp( acos( dot_wn ), 0, M_PI );
		double a = 1 / ( alpha_x * tan( theta_v ) );
		if ( a < 1.6 )
			return ( 3.535 * a + 2.181 * a * a ) / ( 1 + 2.276 * a + 2.577 * a * a );
		else
			return 1;
	}
	return 0;
}

double GGX::D_wh (
	const Vector * wh_local,
	const Vector * wh_global )
{
	double dot_mn = Dot( *wh_local, *wh_global );
	if ( X_plus( dot_mn ) == 1 )
	{
		double theta_m = Clamp( acos( dot_mn ), 0, M_PI );

		if ( std::isinf( tan( theta_m ) ) )
			return 0;

		return ( alpha_x * alpha_x ) /
			(
				M_PI * pow( cos( theta_m ), 4 )
				*
				pow( ( alpha_x * alpha_x + tan( theta_m ) * tan( theta_m ) ), 2 )
			);
	}
	return 0;
}

Vector GGX::sample_wh ()
{
	double r1, r2;
	r1 = getUniformRandomNumber( 0, 1 );
	r2 = getUniformRandomNumber( 0, 1 );

	double theta = atan( alpha_x * sqrt( r1 * ( 1 - r1 ) ) );
	double phi = M_PI * 2 * r2;

	ASSERT_NAN( theta );
	ASSERT_NAN( phi );

	return SphericalDirection( theta, phi );
}

Vector GGX::sample_wh (
	const Vector & wi_global,
	const Vector & wh_global )
{
	return sample_wh();
}

double GGX::masking (
	const Vector * w,
	const Vector * wh_local,
	const Vector * wh_global )
{
	double dot_wm = Dot( *w, *wh_local );
	double dot_wn = Dot( *w, *wh_global );

	if ( X_plus( dot_wm / dot_wn ) == 1 )
	{
		double theta_v = Clamp( acos( dot_wn ), 0, M_PI );
		return 2 / ( 1
			+
			sqrt( 1 + alpha_x * alpha_x * tan( theta_v ) * tan( theta_v ) )
		);
	}
	return 0;
}

// wh_local must be in BRDF coordinate system
double Beckmann_Eric::D_wh (
	const Vector * wh_local,
	const Vector * wh_global )
{
	double tan2Theta = Tan2Theta( *wh_local );
	if ( std::isinf( tan2Theta ) )
		return 0.;
	double cos4Theta = Cos2Theta( *wh_local ) * Cos2Theta( *wh_local );

	return std::exp( -tan2Theta * ( Cos2Phi( *wh_local ) / ( alpha_x * alpha_x ) +
			Sin2Phi( *wh_local ) / ( alpha_y * alpha_y ) ) ) /
		( M_PI * alpha_x * alpha_y * cos4Theta );
}


Vector Beckmann_Eric::sample_wh (
	const Vector & wi_global,
	const Vector & wh_global )
{
	double r1, r2;
	r1 = getUniformRandomNumber( 0, 1 );
	r2 = getUniformRandomNumber( 0, 1 );

	// sample visible normal 
	Vector wh;

	wh = BeckmannSample( -wi_global,
	                     alpha_x,
	                     alpha_x,
	                     r1,
	                     r2 );

	return wh;

	return Vector( 0, 0, 0 );
}

double Beckmann_Eric::masking (
	const Vector * wi,
	const Vector * wo,
	const Vector * wh_local,
	const Vector * wh_global )
{
	return G( *wi, *wo );
}

void Beckmann_Eric::BeckmannSample11 (
	double cosThetaI,
	double U1,
	double U2,
	double * slope_x,
	double * slope_y )
{
	/* Special case (normal incidence) */
	if ( cosThetaI > .9999 )
	{
		if ( U1 == 1 )
			U1 = 1 - eps;
		double r = std::sqrt( -std::log( 1.0f - U1 ) );
		double sinPhi = std::sin( 2 * M_PI * U2 );
		double cosPhi = std::cos( 2 * M_PI * U2 );
		*slope_x = r * cosPhi;
		*slope_y = r * sinPhi;
		return;
	}

	/* The original inversion routine from the paper contained
	discontinuities, which causes issues for QMC integration
	and techniques like Kelemen-style MLT. The following code
	performs a numerical inversion with better behavior */
	double sinThetaI =
		std::sqrt( MAX ( ( double ) 0 , ( double ) 1 - cosThetaI * cosThetaI ) );
	double tanThetaI = sinThetaI / cosThetaI;
	double cotThetaI = 1 / tanThetaI;

	/* Search interval -- everything is parameterized
	in the Erf() domain */
	double a = -1, c = Erf( cotThetaI );
	double sample_x = MAX ( U1 , ( double )1e-6f );

	/* Start with a good initial guess */
	// double  b = (1-sample_x) * a + sample_x * c;

	/* We can do better (inverse of an approximation computed in
	* Mathematica) */
	double thetaI = std::acos( cosThetaI );
	double fit = 1 + thetaI * ( -0.876f + thetaI * ( 0.4265f - 0.0594f * thetaI ) );
	double b = c - ( 1 + c ) * std::pow( 1 - sample_x, fit );

	/* Normalization factor for the CDF */
	static const double SQRT_M_PI_INV = 1.f / std::sqrt( M_PI );
	double normalization =
		1 /
		( 1 + c + SQRT_M_PI_INV * tanThetaI * std::exp( -cotThetaI * cotThetaI ) );

	int it = 0;
	while ( ++it < 10 )
	{
		/* Bisection criterion -- the oddly-looking
		Boolean expression are intentional to check
		for NaNs at little additional cost */
		if ( !( b >= a && b <= c ) ) b = 0.5f * ( a + c );

		/* Evaluate the CDF and its derivative
		(i.e. the density function) */
		double invErf = ErfInv( b );
		double value =
			normalization *
			( 1 + b + SQRT_M_PI_INV * tanThetaI * std::exp( -invErf * invErf ) ) -
			sample_x;
		double derivative = normalization * ( 1 - invErf * tanThetaI );

		if ( std::abs( value ) < 1e-5f ) break;

		/* Update bisection intervals */
		if ( value > 0 )
			c = b;
		else
			a = b;

		b -= value / derivative;
	}

	/* Now convert back into a slope value */
	*slope_x = ErfInv( b );

	/* Simulate Y component */
	*slope_y = ErfInv( 2.0f * MAX ( U2 , ( double )1e-6f ) - 1.0f );

	//			ASSERT ( !std::isinf ( *slope_x ) );
	//			ASSERT ( !std::isnan ( *slope_x ) );
	//			ASSERT ( !std::isinf ( *slope_y ) );
	//			ASSERT ( !std::isnan ( *slope_y ) );
}

Vector Beckmann_Eric::BeckmannSample (
	const Vector & wi,
	double alpha_x,
	double alpha_y,
	double U1,
	double U2 )
{
	// 1. stretch wi
	Vector wiStretched =
		Normalize( Vector( alpha_x * wi.x, alpha_y * wi.y, wi.z ) );

	// 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
	double slope_x, slope_y;
	BeckmannSample11( CosTheta( wiStretched ), U1, U2, &slope_x, &slope_y );

	// 3. rotate
	double tmp = CosPhi( wiStretched ) * slope_x - SinPhi( wiStretched ) * slope_y;
	slope_y = SinPhi( wiStretched ) * slope_x + CosPhi( wiStretched ) * slope_y;
	slope_x = tmp;

	// 4. unstretch
	slope_x = alpha_x * slope_x;
	slope_y = alpha_y * slope_y;

	if ( isnan( slope_x ) || isnan( slope_y ) )
	{
		slope_x = 0;
		slope_y = 0;
	}

	// 5. compute normal
	return Normalize( Vector( -slope_x, -slope_y, 1.f ) );
}

double Beckmann_Eric::Lambda ( const Vector & w ) const
{
	double absTanTheta = std::abs( TanTheta( w ) );

	if ( std::isinf( absTanTheta ) )
		return 0;

	// Compute _alpha_ for direction _w_
	double alpha =
		std::sqrt( Cos2Phi( w ) * alpha_x * alpha_x + Sin2Phi( w ) * alpha_y * alpha_y );

	double a = 1 / ( alpha * absTanTheta );

	if ( a >= 1.6f )
		return 0;
	return ( 1 - 1.259f * a + 0.396f * a * a ) / ( 3.535f * a + 2.181f * a * a );
}

double Beckmann_Eric::D_wh (
	const Vector * wi_local,
	const Vector * wh_local,
	const Vector * wh_global,
	double & pdf_wh )
{
	double D = D_wh( wh_local, wh_global );

	pdf_wh = D * G1( *wi_local ) * AbsDot( *wi_local, *wh_local ) / AbsCosTheta( *wi_local );

	if ( pdf_wh == 0 )
	{
		printf( "" );
	}

	if ( isnan( pdf_wh ) )
	{
		printf( " pdf nan" );
	}

	return D;
}

double Beckmann_Eric::G1 ( const Vector & w ) const
{
	//  if (Dot(w, wh) * CosTheta(w) < 0.) return 0.;
	return 1 / ( 1 + Lambda( w ) );
}

double Beckmann_Eric::G (
	const Vector & wo,
	const Vector & wi ) const
{
	return 1 / ( 1 + Lambda( wo ) + Lambda( wi ) );
}

