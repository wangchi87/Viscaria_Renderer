#include "Attenuation.h"

Attenuation::Attenuation ()
{
	if ( POLARISATION_MODE )
	{
		attenuation_pol = new PolarisingAttenuation [SPECTRUM_NUM];
	}
}

Attenuation::~Attenuation ()
{
}

void Attenuation::Set_Plain_Attenuation ( const Spectrum & s )
{
	attenuation_plain = s;
}

void Attenuation::Set_Polarising_Attenuation ( const PolarisingAttenuation & pol )
{
	if ( POLARISATION_MODE )
		*attenuation_pol = pol;
	else
		printf( "call polarising function in non-polarising mode \n" );
}


void Attenuation::Scaled_by_Spectrum_Value ( const Spectrum & scale )
{
	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		this->attenuation_pol[ i ] =
			( this->attenuation_pol[ i ] ) * scale.spec[ i ];
	else
	LOOP_OVER_SPECTRUM ( i )
		this->attenuation_plain.spec[ i ] =
			scale.spec[ i ] * this->attenuation_plain[ i ];
}

Attenuation Attenuation::operator* ( const double f )
{
	Attenuation output;

	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		output.attenuation_pol[ i ] =
			( this->attenuation_pol[ i ] ) * f;
	else
	LOOP_OVER_SPECTRUM ( i )
		output.attenuation_plain.spec[ i ] =
			this->attenuation_plain[ i ] * f;

	return output;
}

void Attenuation::operator*= ( const double f )
{
	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		this->attenuation_pol[ i ] =
			( this->attenuation_pol[ i ] ) * f;
	else
	LOOP_OVER_SPECTRUM ( i )
		this->attenuation_plain.spec[ i ] =
			this->attenuation_plain[ i ] * f;
}

Attenuation Attenuation::operator/ ( const double f )
{
	return ( *this ) * ( 1 / f );
}

Attenuation Attenuation::operator* ( const Attenuation & f )
{
	Attenuation output;

	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		output.attenuation_pol[ i ] =
			( this->attenuation_pol[ i ] ) * f.attenuation_pol[ i ];
	else
	LOOP_OVER_SPECTRUM ( i )
		output.attenuation_plain.spec[ i ] =
			this->attenuation_plain[ i ] * f.attenuation_plain[ i ];

	return output;
}

Attenuation Attenuation::operator+ ( const Attenuation & f )
{
	Attenuation output;

	if ( POLARISATION_MODE )
	{
		//		LOOP_OVER_SPECTRUM ( i )
		//			output.attenuation_pol[ i ] =
		//			( this->attenuation_pol[ i ] ) + f.attenuation_pol[ i ];
	}
	else
	LOOP_OVER_SPECTRUM ( i )
		output.attenuation_plain.spec[ i ] =
			this->attenuation_plain[ i ] + f.attenuation_plain[ i ];

	return output;
}

void Attenuation::resetZero ()
{
	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		attenuation_pol[ i ].mm.resetZero();
	else
		attenuation_plain.resetZero();
}

void Attenuation::resetOne ()
{
	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		attenuation_pol[ i ].mm.resetUnity();
	else
		attenuation_plain.resetOne();
}

bool Attenuation::isEmpty ()
{
	if ( POLARISATION_MODE )
	{
		LOOP_OVER_SPECTRUM ( i )
			if ( attenuation_pol[ i ].mm.isEmpty() )
				return true;

		return false;
	}
	else
		return attenuation_plain.isEmpty();
}

void Attenuation::print ()
{
	attenuation_plain.print();
}

