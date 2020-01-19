#include "Spectrum.h"

Spectrum::Spectrum ()
{
#ifdef USE_ARRAY
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] = 0;
#endif

#ifndef USE_ARRAY
	spec.resize ( SPECTRUM_NUM );
#endif
	//
	//	if ( POLARISATION_MODE )
	//		spec_pol = new PolarisedLight[ SPECTRUM_NUM ];
}

Spectrum::~Spectrum ()
{
}

Spectrum::Spectrum ( const double * value )
{
#ifdef USE_ARRAY
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] = value[ i ];
#endif

#ifndef USE_ARRAY
	spec.resize ( SPECTRUM_NUM );
	LOOP_OVER_SPECTRUM ( i )
			spec.push_back ( value[i] );
#endif

	//	if (POLARISATION_MODE)
	//		spec_pol = new PolarisedLight[ SPECTRUM_NUM ];
}

void Spectrum::Init_Spectrum ( const Spectrum & s2 )
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] = s2[ i ];
}

void Spectrum::resetZero ()
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] = 0;
}

void Spectrum::resetOne ()
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] = 1;
}

double Spectrum::operator[] ( size_t i ) const
{
	if ( i < SPECTRUM_NUM )
		return spec[ i ];
	else
	{
		printf( "index out of boundary \n" );
		return -1;
	}
}

Spectrum & Spectrum::operator= ( const Spectrum & s2 )
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] = s2.spec[ i ];
	return *this;
}

Spectrum & Spectrum::operator+= ( const Spectrum & s2 )
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] += s2.spec[ i ];
	return *this;
}

Spectrum Spectrum::operator+ ( const Spectrum & s2 )
{
	double * value;
	value = new double[ SPECTRUM_NUM ];

	LOOP_OVER_SPECTRUM ( i )
		value[ i ] = this->spec[ i ] + s2.spec[ i ];
	return Spectrum( value );
}

Spectrum & Spectrum::operator*= ( const double f )
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] *= f;
	return *this;
}

Spectrum & Spectrum::operator/= ( const double f )
{
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] /= f;
	return *this;
}

Spectrum Spectrum::operator* ( const double f )
{
	Spectrum new_spectrum;
	LOOP_OVER_SPECTRUM ( i )
		new_spectrum.spec[ i ] = this->spec[ i ] * f;
	return new_spectrum;
}

Spectrum Spectrum::operator+ ( const double f )
{
	Spectrum new_spectrum;
	LOOP_OVER_SPECTRUM ( i )
		new_spectrum.spec[ i ] = this->spec[ i ] + f;
	return new_spectrum;
}


Spectrum Spectrum::operator/ ( const double f )
{
	Spectrum new_spectrum;
	LOOP_OVER_SPECTRUM ( i )
		new_spectrum.spec[ i ] = this->spec[ i ] / f;
	return new_spectrum;
}

void Spectrum::print ()
{
	LOOP_OVER_SPECTRUM ( i )
		printf( "%f ", spec[ i ] );
	printf( "\n" );
}

bool Spectrum::isEmpty () const
{
	LOOP_OVER_SPECTRUM ( i )
		if ( spec[ i ] == 0 )
			return true;
	return false;
}

bool Spectrum::HasNaNs ()
{
	LOOP_OVER_SPECTRUM ( i )
		if ( _isnan( spec[ i ] ) )
			return true;
	return false;
}

double Spectrum::Length ()
{
	double length = 0;
	LOOP_OVER_SPECTRUM ( i )
		length += spec[ i ] * spec[ i ];
	length = sqrt( length );
	return length;
}

bool Spectrum::Normalise ()
{
	double length = Length();
	LOOP_OVER_SPECTRUM ( i )
		spec[ i ] /= length;
	return false;
}

