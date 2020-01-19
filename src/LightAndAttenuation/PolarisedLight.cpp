#include "PolarisedLight.h"

PolarisedLight::PolarisedLight ()
{
	sv = StokesVectorUnitUnpol();

	ref_frame.InitRandomReferenceFrame( Vector( 0, 0, 1 ) );
}

PolarisedLight::PolarisedLight ( const ReferenceFrame & rf )
{
	sv = StokesVectorUnitUnpol();
	ref_frame = rf;
}

PolarisedLight::PolarisedLight ( const StokesVector & sv_vector, const ReferenceFrame & rf )
{
	sv = sv_vector;
	ref_frame = rf;
}

PolarisedLight::PolarisedLight ( const StokesVector & sv_vector, const Vector & dir, const Vector & n )
{
	sv = sv_vector;
	Set_Reference_Frame( dir, n );
}

void PolarisedLight::Set_StokesVector ( const StokesVector & sv_vector )
{
	sv = sv_vector ;
}

void PolarisedLight::Set_Reference_Frame ( const ReferenceFrame & rf )
{
	ref_frame = rf;
}

void PolarisedLight::Set_Reference_Frame ( const Vector & dir, const Vector & n )
{
	ref_frame = ReferenceFrame( dir, n );
}

ReferenceFrame PolarisedLight::get_ReferenceFrame () const
{
	return this->ref_frame;
}

StokesVector PolarisedLight::get_StokesVector () const
{
	return this->sv;
}

double PolarisedLight::get_intensity () const
{
	return sv.intensity();
}

void PolarisedLight::Print ()
{
	printf( "\nPolarised Light : \n" );
	sv.Print();
	ref_frame.print();
}

PolarisedLight PolarisedLight::operator += ( const PolarisedLight & pl )
{
	// test colinearity
	if ( isCoLinear( ref_frame, pl.get_ReferenceFrame(), 3 ) )
	{
		if ( ref_frame.Valid_ReferenceFrame() || sv.isNotEmpty() )
		{
			double rotationAngle = Angle_Between_Two_Refs( pl.get_ReferenceFrame(), this->ref_frame );

			StokesVector rotated_sv;

			// apply rotation to input parameter "pl"
			pl.get_StokesVector().rotate( rotationAngle, rotated_sv );

			sv = sv + rotated_sv;

			return ( *this );
		}
		else
		{
			if ( sv.isNotEmpty() )
				printf( "incorrect polarised light with empty reference frame" );

			return pl;
		}
	}
	else
	{
		// not co-linear light
		// addition is meaningless
		return *this;
	}
}

PolarisedLight PolarisedLight::operator+ ( const PolarisedLight & pl )
{
	PolarisedLight new_pl = *this;

	StokesVector new_sv = new_pl.get_StokesVector();
	ReferenceFrame new_rf = new_pl.get_ReferenceFrame();

	// test colinearity
	if ( isCoLinear( new_rf, pl.get_ReferenceFrame(), 3 ) )
	{
		if ( new_rf.Valid_ReferenceFrame() || new_sv.isNotEmpty() )
		{
			double rotationAngle = Angle_Between_Two_Refs( pl.get_ReferenceFrame(), new_rf );

			StokesVector rotated_sv;

			// apply rotation to input parameter "pl"
			pl.get_StokesVector().rotate( rotationAngle, rotated_sv );

			new_sv = sv + rotated_sv;

			new_pl.Set_StokesVector( new_sv );

			return new_pl ;
		}
		else
		{
			if ( sv.isNotEmpty() )
				printf( "incorrect polarised light with empty reference frame" );

			return pl;
		}
	}
	else
	{
		// not co-linear light
		// addition is meaningless
		printf( "non-colinear polarisation addition" );
		return *this;
	}
}

PolarisedLight PolarisedLight::operator = ( const PolarisedLight & pl )
{
	sv = pl.get_StokesVector();
	ref_frame = pl.get_ReferenceFrame();
	return *this;
}

PolarisedLight PolarisedLight::operator *= ( const double d )
{
	sv = sv * d;
	return *this;
}

void PolarisedLight::Align_with_New_ReferenceFrame ( const ReferenceFrame & ref )
{
	double rotationAngle = Angle_Between_Two_Refs( this->ref_frame, ref );

	sv.rotate( rotationAngle );
	ref_frame = ref;
}

