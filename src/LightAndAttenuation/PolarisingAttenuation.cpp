#include "PolarisingAttenuation.h"


PolarisingAttenuation::PolarisingAttenuation ()
{
	mm.resetUnity();
}


PolarisingAttenuation::~PolarisingAttenuation ()
{
}

PolarisingAttenuation::PolarisingAttenuation (
	const MuellerMatrix & mm_matrix,
	const ReferenceFrame & rf_in,
	const ReferenceFrame & rf_out )
{
	mm = mm_matrix;
	entry_rf = rf_in;
	exit_rf = rf_out;
}

// rotate incident polarisation, 
// use the exit_rf of Muellle matrix as the ref.frame of outgoing ray
PolarisedLight PolarisingAttenuation::operator* ( const PolarisedLight & pl_in )
{
	if ( !this->isValid() )
	{
		printf( "this attenuation is not valid" );
		return pl_in;
	}

	if ( isCoLinear( entry_rf, pl_in.get_ReferenceFrame(), 3 ) )
	{
		// calculation rotation angle from ref frame of incidence to the entry ref frame of Mueller matrix
		double rotation_angle = Angle_Between_Two_Refs( pl_in.get_ReferenceFrame(), entry_rf );

		StokesVector sv_rotated_in;
		pl_in.get_StokesVector().rotate( rotation_angle, sv_rotated_in );

		// multipy mueller matrix by stokes vector 
		StokesVector sv_out = mm * sv_rotated_in;

		PolarisedLight pl_out( sv_out, exit_rf );

		return pl_out;
	}
	else
	{
		printf( "not co-linear mm * stokes vector multiplication\n" );
		return PolarisedLight();
	}
}

PolarisingAttenuation PolarisingAttenuation::operator* ( const double d )
{
	mm = mm * d;
	return *this;
}

PolarisingAttenuation PolarisingAttenuation::operator= ( const PolarisingAttenuation & pa )
{
	mm = pa.mm;
	exit_rf = pa.exit_rf;
	entry_rf = pa.entry_rf;

	return *this;
}

// right multiply an polarising attenuation
// keep current exit_rf, and use input entry_rf as new extry ref.frame
PolarisingAttenuation PolarisingAttenuation::operator *= ( const PolarisingAttenuation & atten_input )
{
	if ( !this->isValid() )
	{
		printf( "this attenuation is not valid" );
		return atten_input;
	}

	if ( isCoLinear( this->entry_rf, atten_input.exit_rf, 3 ) )
	{
		double rotation_angle = Angle_Between_Two_Refs( atten_input.exit_rf, this->entry_rf );

		MuellerMatrix mm_in_rotated = atten_input.mm.rotate( rotation_angle );

		this->mm *= mm_in_rotated;

		Set_Reference_Frame( atten_input.entry_rf, this->exit_rf );

		return *this;
	}
	else
	{
		printf( "not co-linear mm * mm multiplication \n" );
		return *this;
	}
}

PolarisingAttenuation PolarisingAttenuation::operator * ( const PolarisingAttenuation & atten_input )
{
	if ( !this->isValid() )
	{
		printf( "this attenuation is not valid" );
		return atten_input;
	}


	PolarisingAttenuation new_att = *this;

	if ( isCoLinear( new_att.entry_rf, atten_input.exit_rf, 3 ) )
	{
		double rotation_angle = Angle_Between_Two_Refs( atten_input.exit_rf, new_att.entry_rf );

		MuellerMatrix mm_in_rotated = atten_input.mm.rotate( rotation_angle );

		new_att.mm *= mm_in_rotated;

		new_att.Set_Reference_Frame( atten_input.entry_rf, new_att.exit_rf );

		return new_att;
	}
	else
	{
		printf( "not co-linear mm * mm multiplication \n" );
		return *this;
	}
}


void PolarisingAttenuation::Set_Mueller_Matrix ( const MuellerMatrix & mm_matrix )
{
	mm = mm_matrix;
}

void PolarisingAttenuation::Set_Reference_Frame ( const ReferenceFrame & rf_in, const ReferenceFrame & rf_out )
{
	entry_rf = rf_in;
	exit_rf = rf_out;
}

void PolarisingAttenuation::InitFresnelPolarisingAttenuation (
	const double & A,
	const double & B,
	const double & C,
	const double & S,
	const Vector & in_dir,
	const Vector & out_dir,
	const Vector & surface_normal )
{
	Set_Frensel_MuellerMatrix( mm, A, B, C, S );

	entry_rf.Set_Reference_Frame( in_dir, surface_normal );
	exit_rf.Set_Reference_Frame( out_dir, surface_normal );
}

bool PolarisingAttenuation::isValid ()
{
	if ( entry_rf.Valid_ReferenceFrame()
		&&
		exit_rf.Valid_ReferenceFrame()
		&& !mm.isEmpty()
	)
		return true;
	else
		return false;
}

