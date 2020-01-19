/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:10
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:10
 * @Description: All rights reserved
 */
#pragma once

// most of the code in this file is integrated from SCATMECH


#include "Foundations/Vector.h"
#include <math.h>
#include <complex> 

#ifndef MATH_RAD_TO_DEG
#define MATH_RAD_TO_DEG 180/M_PI
#endif

#ifndef MATH_DEG_TO_RAD
#define MATH_DEG_TO_RAD M_PI/180
#endif

using namespace std;

typedef std::complex<double> STD_COMPLEX;

class JonesMat;
class JonesVec;
class MuellerMatrix;
class StokesVector;


class JonesVec
{
public:

	JonesVec ()
	{
	}

	JonesVec ( const JonesVec & x )
	{
		j[ 0 ] = x.j[ 0 ];
		j[ 1 ] = x.j[ 1 ];
	}

	/// Constructor taking two STD_COMPLEX values
	JonesVec ( const STD_COMPLEX & s, const STD_COMPLEX & p )
	{
		j[ 0 ] = s;
		j[ 1 ] = p;
	}

	/// Conversion constructor from StokesVector
	explicit JonesVec ( const StokesVector & x );

	JonesVec & operator= ( const JonesVec & x )
	{
		j[ 0 ] = x.j[ 0 ];
		j[ 1 ] = x.j[ 1 ];
		return *this;
	}

	JonesVec operator* ( const STD_COMPLEX & x ) const
	{
		return JonesVec( j[ 0 ] * x, j[ 1 ] * x );
	}

	friend JonesVec operator* ( const STD_COMPLEX & x, const JonesVec & y )
	{
		return y * x;
	}

	JonesVec operator/ ( const STD_COMPLEX & x ) const
	{
		return ( 1. / x ) * ( *this );
	}

	JonesVec & operator*= ( const STD_COMPLEX & x )
	{
		return *this = x * ( *this );
	}

	JonesVec & operator/= ( const STD_COMPLEX & x )
	{
		return *this = ( 1. / x ) * ( *this );
	}

	// Addition operators...
	JonesVec operator+ ( const JonesVec & a ) const
	{
		return JonesVec( j[ 0 ] + a.j[ 0 ], j[ 1 ] + a.j[ 1 ] );
	}

	JonesVec operator- ( const JonesVec & a ) const
	{
		return JonesVec( j[ 0 ] - a.j[ 0 ], j[ 1 ] - a.j[ 1 ] );
	}

	JonesVec operator+= ( const JonesVec & a )
	{
		return *this = *this + a;
	}

	JonesVec operator-= ( const JonesVec & a )
	{
		return *this = *this - a;
	}

	JonesVec operator- () const
	{
		return JonesVec( -j[ 0 ], -j[ 1 ] );
	}

	JonesVec operator+ () const
	{
		return *this;
	}

	/// Element indexing
	STD_COMPLEX & operator[] ( int i )
	{
		return j[ i ];
	}

	const STD_COMPLEX & operator[] ( int i ) const
	{
		return j[ i ];
	}

	/// Return intensity
	double intensity () const
	{
		return std::norm( j[ 0 ] ) + std::norm( j[ 1 ] );
	}

	/// arctan of the component ratio
	double psi () const
	{
		return atan( std::abs( j[ 1 ] ) / std::abs( j[ 0 ] ) );
	}

	/// phase between two components
	double delta () const
	{
		return std::arg( j[ 1 ] ) - std::arg( j[ 0 ] );
	}

	/// principle angle of polarization
	double eta () const;

	/// degree of linear polarization
	double DOLP () const;

	/// degree of polarization (always 1 for Jones!)
	double DOP () const
	{
		return 1.;
	}

	/// degree of circular polarization
	double DOCP () const;

	/// ellipticity (ratio of minor to major axes)
	double e () const;

	/// eccentricity [sqrt(1-e^2)]
	double epsilon () const
	{
		double t = e();
		return sqrt( 1. - t * t );
	}

	/// Vector rotated by angle...
	JonesVec rotate ( const double angle ) const;

	/// S component of wave
	STD_COMPLEX & S ()
	{
		return j[ 0 ];
	}

	const STD_COMPLEX & S () const
	{
		return j[ 0 ];
	}

	/// P component of wave
	STD_COMPLEX & P ()
	{
		return j[ 1 ];
	}

	const STD_COMPLEX & P () const
	{
		return j[ 1 ];
	}

	friend class JonesMat;
	friend class MuellerMatrixMatrix;
	friend class StokesVector;

public:

	// The elements of the vector...
	STD_COMPLEX j[2];
};

/**
 * Class to handle Jones Matrices...
 * Class to store the elements and handle operations of a Jones matrix
 */
class JonesMat
{
public:
	JonesMat ()
	{
	}

	JonesMat ( const JonesMat & x )
	{
		j[ 0 ] = x.j[ 0 ];
		j[ 1 ] = x.j[ 1 ];
		j[ 2 ] = x.j[ 2 ];
		j[ 3 ] = x.j[ 3 ];
	}

	JonesMat ( const STD_COMPLEX & pp, const STD_COMPLEX & ss,
	           const STD_COMPLEX & ps, const STD_COMPLEX & sp )
	{
		j[ 0 ] = pp;
		j[ 1 ] = ss;
		j[ 2 ] = ps , j[ 3 ] = sp;
	}

	explicit JonesMat ( const MuellerMatrix & x );

	JonesMat & operator= ( const JonesMat & x )
	{
		j[ 0 ] = x.j[ 0 ];
		j[ 1 ] = x.j[ 1 ];
		j[ 2 ] = x.j[ 2 ];
		j[ 3 ] = x.j[ 3 ];
		return *this;
	}

	// Multiplication operators...
	JonesMat operator* ( const JonesMat & matrix ) const
	{
		return JonesMat( j[ 3 ] * matrix.j[ 2 ] + j[ 0 ] * matrix.j[ 0 ],
		                 j[ 1 ] * matrix.j[ 1 ] + j[ 2 ] * matrix.j[ 3 ],
		                 j[ 1 ] * matrix.j[ 2 ] + j[ 2 ] * matrix.j[ 0 ],
		                 j[ 3 ] * matrix.j[ 1 ] + j[ 0 ] * matrix.j[ 3 ] );
	}

	JonesMat operator* ( const STD_COMPLEX & x ) const
	{
		return JonesMat( j[ 0 ] * x, j[ 1 ] * x, j[ 2 ] * x, j[ 3 ] * x );
	}

	friend JonesMat operator* ( const STD_COMPLEX & x, const JonesMat & y )
	{
		return y * x;
	}

	JonesMat operator/ ( const STD_COMPLEX & x ) const
	{
		return JonesMat( j[ 0 ] / x, j[ 1 ] / x, j[ 2 ] / x, j[ 3 ] / x );
	}

	JonesMat & operator*= ( const JonesMat & a )
	{
		return *this = *this * a;
	}

	JonesMat & operator*= ( const STD_COMPLEX & a )
	{
		return *this = *this * a;
	}

	JonesMat & operator/= ( const STD_COMPLEX & a )
	{
		return *this = *this / a;
	}

	JonesVec operator* ( const JonesVec & a ) const
	{
		return JonesVec( j[ 1 ] * a.j[ 0 ] + j[ 2 ] * a.j[ 1 ], j[ 3 ] * a.j[ 0 ] + j[ 0 ] * a.j[ 1 ] );
	}

	// Addition operators...
	JonesMat operator+ ( const JonesMat & a ) const
	{
		return JonesMat( j[ 0 ] + a.j[ 0 ], j[ 1 ] + a.j[ 1 ], j[ 2 ] + a.j[ 2 ], j[ 3 ] + a.j[ 3 ] );
	}

	JonesMat operator- ( const JonesMat & a ) const
	{
		return JonesMat( j[ 0 ] - a.j[ 0 ], j[ 1 ] - a.j[ 1 ], j[ 2 ] - a.j[ 2 ], j[ 3 ] - a.j[ 3 ] );
	}

	JonesMat & operator+= ( const JonesMat & a )
	{
		return *this = *this + a;
	}

	JonesMat & operator-= ( const JonesMat & a )
	{
		return *this = *this - a;
	}

	JonesMat operator- () const
	{
		return JonesMat( -j[ 0 ], -j[ 1 ], -j[ 2 ], -j[ 3 ] );
	}

	JonesMat operator+ () const
	{
		return *this;
	}

	/// Element indexing
	STD_COMPLEX & operator[] ( int i )
	{
		return j[ i ];
	};

	/// Constant element indexing
	const STD_COMPLEX & operator[] ( int i ) const
	{
		return j[ i ];
	};

	/// Return matrix rotated by angle
	JonesMat rotate ( double angle ) const;

	/// Return matrix transpose
	JonesMat transpose () const
	{
		return JonesMat( j[ 0 ], j[ 1 ], j[ 3 ], j[ 2 ] );
	}

	/// p->p matrix element
	STD_COMPLEX & PP ()
	{
		return j[ 0 ];
	}

	const STD_COMPLEX & PP () const
	{
		return j[ 0 ];
	}

	/// s->s matrix element
	STD_COMPLEX & SS ()
	{
		return j[ 1 ];
	}

	const STD_COMPLEX & SS () const
	{
		return j[ 1 ];
	}

	/// p->s matrix element
	STD_COMPLEX & PS ()
	{
		return j[ 2 ];
	}

	const STD_COMPLEX & PS () const
	{
		return j[ 2 ];
	}

	/// s->p matrix element
	STD_COMPLEX & SP ()
	{
		return j[ 3 ];
	}

	const STD_COMPLEX & SP () const
	{
		return j[ 3 ];
	}


	friend class JonesVec;
	friend class MuellerMatrixMat;
	friend class StokesVector;
public:

	// Array of matrix elements...
	// (Note that the elements are p->p, s->s, p->s, and s->p, in order)
	STD_COMPLEX j[4];
};

class MuellerMatrix
{
public:

	// Constructors and Assignments...
	MuellerMatrix ();


	MuellerMatrix ( const JonesMat & j );

	MuellerMatrix ( const MuellerMatrix & x )
	{
		*this = x;
	}

	MuellerMatrix ( const StokesVector & s1, const StokesVector & s2, const StokesVector & s3, const StokesVector & s4 );
	MuellerMatrix & operator= ( const MuellerMatrix & x );

	// Element indexing...
	double * operator[] ( int i )
	{
		return m[ i ];
	}

	const double * operator[] ( int i ) const
	{
		return m[ i ];
	}

	/// Maximum "transmission"...
	double Tmax () const
	{
		return m[ 0 ][ 0 ] + sqrt( m[ 0 ][ 1 ] * m[ 0 ][ 1 ] + m[ 0 ][ 2 ] * m[ 0 ][ 2 ] + m[ 0 ][ 3 ] * m[ 0 ][ 3 ] );
	}

	bool isEmpty ();

	void resetUnity ();

	void resetZero ();

	void SetValue ( int i, int j, double v )
	{
		m[ i ][ j ] = v;
	}

	/// Minimum "transmission"...
	double Tmin () const
	{
		return m[ 0 ][ 0 ] - sqrt( m[ 0 ][ 1 ] * m[ 0 ][ 1 ] + m[ 0 ][ 2 ] * m[ 0 ][ 2 ] + m[ 0 ][ 3 ] * m[ 0 ][ 3 ] );
	}

	/// Diattenuation...
	double diattenuation () const
	{
		return sqrt( m[ 0 ][ 1 ] * m[ 0 ][ 1 ] + m[ 0 ][ 2 ] * m[ 0 ][ 2 ] + m[ 0 ][ 3 ] * m[ 0 ][ 3 ] ) / m[ 0 ][ 0 ];
	}

	/// Linear diattenuation...
	double linear_diattenuation () const
	{
		return sqrt( m[ 0 ][ 1 ] * m[ 0 ][ 1 ] + m[ 0 ][ 2 ] * m[ 0 ][ 2 ] ) / m[ 0 ][ 0 ];
	}

	/// Polarization dependent loss...
	double polarization_dependent_loss () const
	{
		return 10. * log( Tmax() / Tmin() ) / log( 10. );
	}

	/// Polarizance...
	double polarizance () const
	{
		return m[ 0 ][ 0 ] + sqrt( m[ 1 ][ 0 ] * m[ 1 ][ 0 ] + m[ 2 ][ 0 ] * m[ 2 ][ 0 ] + m[ 3 ][ 0 ] * m[ 3 ][ 0 ] );
	}

	/// Extinction ratio...
	double extinction_ratio () const
	{
		return Tmax() / Tmin();
	}

	/// Return matrix rotated by angle...
	MuellerMatrix rotate ( const double angle ) const;
	/// Return scattering matrix for a parity conversion
	MuellerMatrix parity () const;
	/// Return transpose of matrix...
	MuellerMatrix transpose () const;

	// Binary Arithmetic Operations...
	MuellerMatrix operator* ( const MuellerMatrix & matrix ) const;

	MuellerMatrix & operator*= ( const MuellerMatrix & matrix )
	{
		return *this = *this * matrix;
	}

	MuellerMatrix operator* ( double d ) const;

	StokesVector operator* ( const StokesVector & s ) const;

	MuellerMatrix & operator*= ( const double d )
	{
		return *this = *this * d;
	}

	MuellerMatrix operator/ ( double d ) const;

	MuellerMatrix & operator/= ( double d )
	{
		return *this = *this * ( 1. / d );
	}

	MuellerMatrix operator+ ( const MuellerMatrix & a ) const;
	MuellerMatrix operator- ( const MuellerMatrix & a ) const;
	MuellerMatrix operator- () const;

	MuellerMatrix operator+ () const
	{
		return *this;
	}

	MuellerMatrix & operator+= ( const MuellerMatrix & matrix )
	{
		return *this = *this + matrix;
	}

	MuellerMatrix & operator-= ( const MuellerMatrix & matrix )
	{
		return *this = *this - matrix;
	}

	friend class JonesVec;
	friend class JonesMat;
	friend class StokesVector;

public:

	// The elements of the MuellerMatrix matrix...
	double m[4][4];
};

class StokesVector
{
public:

	StokesVector ();

	void Print ();

	StokesVector ( const JonesVec & j );

	StokesVector ( double I, double Q, double U, double V )
	{
		s[ 0 ] = I;
		s[ 1 ] = Q;
		s[ 2 ] = U;
		s[ 3 ] = V;
	}

	StokesVector ( const StokesVector & x );

	StokesVector operator* ( const MuellerMatrix & matrix ) const;
	StokesVector & operator= ( const StokesVector & x );

	// Aliases for each element...

	/// The first elements (intensity) of the Stokes vector
	double & I ()
	{
		return s[ 0 ];
	}

	double I () const
	{
		return s[ 0 ];
	}

	/// The second element (Is - Ip) of the Stokes vector
	double & Q ()
	{
		return s[ 1 ];
	}

	double Q () const
	{
		return s[ 1 ];
	}

	/// The third element [I(45) - I(-45)] of the Stokes vector
	double & U ()
	{
		return s[ 2 ];
	}

	double U () const
	{
		return s[ 2 ];
	}

	/// The fourth element [I(lcp) - I(rcp)] of the Stokes vector
	double & V ()
	{
		return s[ 3 ];
	}

	double V () const
	{
		return s[ 3 ];
	}

	/// The i-th element (zero indexing)
	double & operator[] ( int i )
	{
		return s[ i ];
	}

	double operator[] ( int i ) const
	{
		return s[ i ];
	}

	// Binary arithmetic operations...
	//StokesVector operator*(const MuellerMatrix& matrix) const;
	double operator * ( const StokesVector & a ) const
	{
		return a.s[ 0 ] * s[ 0 ] + a.s[ 1 ] * s[ 1 ] + a.s[ 2 ] * s[ 2 ] + a.s[ 3 ] * s[ 3 ];
	}

	StokesVector operator + ( const StokesVector & a ) const
	{
		return StokesVector( s[ 0 ] + a.s[ 0 ], s[ 1 ] + a.s[ 1 ], s[ 2 ] + a.s[ 2 ], s[ 3 ] + a.s[ 3 ] );
	}

	StokesVector operator - ( const StokesVector & a ) const
	{
		return StokesVector( s[ 0 ] - a.s[ 0 ], s[ 1 ] - a.s[ 1 ], s[ 2 ] - a.s[ 2 ], s[ 3 ] - a.s[ 3 ] );
	}

	StokesVector operator - () const
	{
		return StokesVector( -s[ 0 ], -s[ 1 ], -s[ 2 ], -s[ 3 ] );
	}

	StokesVector operator+ () const
	{
		return *this;
	}

	StokesVector operator * ( double d ) const
	{
		return StokesVector( s[ 0 ] * d, s[ 1 ] * d, s[ 2 ] * d, s[ 3 ] * d );
	}

	friend StokesVector operator* ( double d, const StokesVector & s )
	{
		return StokesVector( s[ 0 ] * d, s[ 1 ] * d, s[ 2 ] * d, s[ 3 ] * d );
	}

	StokesVector & operator *= ( double d )
	{
		return *this = *this * d;
	}

	StokesVector operator/ ( double d ) const
	{
		return StokesVector( s[ 0 ] / d, s[ 1 ] / d, s[ 2 ] / d, s[ 3 ] / d );
	}

	StokesVector & operator/= ( double d )
	{
		return *this = *this / d;
	}

	// rotated by given angle...
	void rotate ( double angle, StokesVector & sv_out ) const;

	void rotate ( double angle );

	/// Principle angle of polarization
	double eta () const
	{
		return atan2( s[ 2 ], s[ 1 ] ) / 2.;
	}

	/// Intensity
	double intensity () const
	{
		return s[ 0 ];
	}

	/// Degree of linear polarization
	double DOLP () const
	{
		return sqrt( Sqr( s[ 2 ] ) + Sqr( s[ 1 ] ) ) / s[ 0 ];
	}

	/// Degree of polarization 
	double DOP () const
	{
		return sqrt( Sqr( s[ 2 ] ) + Sqr( s[ 1 ] ) + Sqr( s[ 3 ] ) ) / s[ 0 ];
	}

	/// Degree of circular polarization
	double DOCP () const
	{
		return s[ 3 ] / s[ 0 ];
	}

	/// Ellipticity (ratio of minor to major axes)
	double e () const
	{
		return s[ 3 ] / ( s[ 0 ] + sqrt( Sqr( s[ 1 ] ) + Sqr( s[ 2 ] ) ) );
	}

	/// Phase between two components
	double delta () const;

	/// Arctangent of the component ratio
	double psi () const;

	/// eccentricity [sqrt(1-e^2)]
	double eccentricity () const
	{
		return sqrt( 1. - Sqr( this->e() ) );
	}

	/// Polarized part of Stokes Vector
	StokesVector pol_part () const;

	/// Unpolarized part of Stokes Vector
	StokesVector unpol_part () const;

	bool isNotEmpty () const
	{
		if ( s[ 0 ] != 0 || s[ 1 ] != 0 || s[ 2 ] != 0 || s[ 3 ] != 0 )
			return true;
		else return false;
	}

	friend class JonesVec;
	friend class MuellerMatrix;
	friend class JonesMat;

public:

	// The elements of the Stokes vector...
	double s[4];
};

MuellerMatrix MuellerMatrix_Zero ();
MuellerMatrix MuellerMatrix_Unit ( double attenuation = 1. );
MuellerMatrix MuellerMatrix_Depolarizer ( double attenuation = 1., double depolarization = 1. );
MuellerMatrix MuellerMatrix_PartialLinearPolarizer ( double tmax, double tmin, double angle );

inline StokesVector StokesVectorUnitUnpol ()
{
	return StokesVector( 1, 0, 0, 0 );
}

inline StokesVector StokesVectorUnitS ()
{
	return StokesVector( 1, 1, 0, 0 );
}

inline StokesVector StokesVectorUnitP ()
{
	return StokesVector( 1, -1, 0, 0 );
}

inline StokesVector StokesVectorUnitLinear ( double eta )
{
	return StokesVector( 1, cos( 2 * eta ), sin( 2 * eta ), 0 );
}

inline StokesVector StokesVectorUnitRCP ()
{
	return StokesVector( 1, 0, 0, -1 );
}

inline StokesVector StokesVectorUnitLCP ()
{
	return StokesVector( 1, 0, 0, 1 );
}

inline StokesVector StokesVectorUnitGeneral ( double eta, double DOCP = 0., double DOP = 1. )
{
	double temp = sqrt( 1. - Sqr( DOCP ) );
	return StokesVector( 1,
	                     DOP * temp * cos( 2 * eta ),
	                     DOP * temp * sin( 2 * eta ),
	                     DOP * DOCP );
}

inline JonesMat JonesZero ()
{
	return JonesMat( 0., 0., 0., 0. );
}

JonesMat Jones_Rotator ( double angle );
JonesMat Jones_LinearRetarder ( double phase, double angle = 0 );
JonesMat Jones_CircularRetarder ( double phase );
JonesMat Jones_LinearPolarizer ( double angle = 0, double diattenuation = 1 );
JonesMat Jones_CircularPolarizer ( double diattenuation = 1 );

// Returns Jones matrix given eigenvectors and eigenvalues...
JonesMat JonesGeneralized ( const JonesVec & a, const JonesVec & b,
                            const STD_COMPLEX & ma, const STD_COMPLEX & mb );

inline JonesVec Jones_VecUnitS ()
{
	return JonesVec( 1., 0. );
}

inline JonesVec Jones_VecUnitP ()
{
	return JonesVec( 0., 1. );
}

inline JonesVec Jones_VecUnitLinear ( double eta )
{
	return JonesVec( cos( eta ), sin( eta ) );
}

inline JonesVec Jones_VecUnitRCP ()
{
	return JonesVec( 1., STD_COMPLEX( 0, -1 ) );
}

inline JonesVec Jones_VecUnitLCP ()
{
	return JonesVec( 1., STD_COMPLEX( 0, 1 ) );
}

inline JonesVec Jones_VecUnitGeneral ( double eta, double DOCP )
{
	return JonesVec( StokesVectorUnitGeneral( eta, DOCP ) );
}


class ReferenceFrame
{
public:
	// refrence frame 
	Vector v1;
	Vector v2;

	// light direction
	Vector d;

	ReferenceFrame ()
	{
		v1 = Vector( 0, 0, 0 );
		v2 = Vector( 0, 0, 0 );
	}

	ReferenceFrame ( const ReferenceFrame & rf1 )
	{
		v1 = rf1.v1;
		v2 = rf1.v2;
	}

	ReferenceFrame ( const Vector dir, const Vector n )
	{
		Set_Reference_Frame( dir, n );
	}

	void InitRandomReferenceFrame ( const Vector dir )
	{
		d = dir;
		CoordinateSystem( dir, &v1, &v2 );

		v1 = Normalize( v1 );
		v2 = Normalize( v2 );
	}

	// n is the vector which helps to establish the ref frame.
	// for instance, on a microsurface, if n is micro-normal vector
	// the generated ref frame is local polarisation ref frame
	// if n is macro-normal vector,then it will be global polarisation ref frame.
	void Set_Reference_Frame ( const Vector dir, const Vector n )
	{
		d = dir;

		if ( AbsDot( dir, n ) == 0 )
		{
			InitRandomReferenceFrame( dir );
			return;
		}

		v1 = Cross( d, n );
		v2 = Cross( d, v1 );

		v1 = Normalize( v1 );
		v2 = Normalize( v2 );
	}

	Vector Get_Light_Direction () const
	{
		return d;
	}

	ReferenceFrame operator = ( const ReferenceFrame & rf )
	{
		v1 = rf.v1;
		v2 = rf.v2;
		return *this;
	}

	bool Valid_ReferenceFrame ()
	{
		if ( v1.isEmpty() || v2.isEmpty() )
			return false;
		else
			return true;
	}

	void print ()
	{
		printf( "Ref-frame 1 : " );
		v1.Print();
		printf( "Ref-frame 2 : " );
		v2.Print();
	}
};

// rotation angle from rf1 to rf2
// the sequence ( from rf1 to rf2 OR from rf2 to rf1 ) is IMPORTANT
inline double Angle_Between_Two_Refs (
	const ReferenceFrame & rf1,
	const ReferenceFrame & rf2 )
{
	double angle = acos( Dot( rf1.v1, rf2.v1 ) );
	double sign = Dot( rf1.v2, rf2.v1 );

	if ( sign > 0 )
		return angle;
	else
		return -angle;
}

inline bool isCoLinear (
	const ReferenceFrame & rf1,
	const ReferenceFrame & rf2,
	const double tolerance )
{
	double isColinearLight = AbsDot( rf1.d, rf2.d );

	// if angle is less than 'tolerance' degree, 
	// let's say they are co-linear
	if ( isColinearLight < tolerance * MATH_DEG_TO_RAD )
		return true;
	else
		return false;
}

inline void Set_Frensel_MuellerMatrix ( MuellerMatrix & mm, double A, double B, double C, double S )
{
	double T = -S;

	// avoid -0 
	if ( S == 0 )
		T = 0;

	mm.SetValue( 0, 0, A );
	mm.SetValue( 0, 1, B );
	mm.SetValue( 1, 0, B );
	mm.SetValue( 1, 1, A );
	mm.SetValue( 2, 2, C );
	mm.SetValue( 2, 3, S );
	mm.SetValue( 3, 2, T );
	mm.SetValue( 3, 3, C );
}

