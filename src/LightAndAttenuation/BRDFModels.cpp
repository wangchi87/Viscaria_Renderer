#include "BRDFModels.h"
#include <random>

BRDFModels::BRDFModels ( void )
{
	surfaceFlag = surface_not_specified;
}


BRDFModels::~BRDFModels ( void )
{
}


SpecularReflection::SpecularReflection (
	Spectrum n1,
	Spectrum k1 )
{
	surfaceFlag = surface_delta;
	refractionIndex_n = n1;
	refractionIndex_k = k1;
}

void SpecularReflection::BRDF (
	Vector in,
	Vector out,
	Attenuation & value
)
{
	Vector local_normal = Normalize( out - in );

	double cos_theta = AbsDot( in, local_normal );
	double theta = Clamp( acos( cos_theta ), 0, M_PI );

	LOOP_OVER_SPECTRUM( i )
		value.attenuation_plain.spec[ i ] =
			Fresnel_Reflection_Plain(
				theta,
				1, 0,
				refractionIndex_n.spec[ i ],
				refractionIndex_k.spec[ i ] );

	value = value / ( cos_theta );

	ASSERT_NAN( value );
}

void SpecularReflection::BRDF (
	Vector in,
	Vector normal,
	Vector out,
	Attenuation & value,
	double & pdf )
{
	double theta_i = acos( AbsDot( in, normal ) );
	double theta_o = acos( AbsDot( out, normal ) );

	if ( abs( theta_i - theta_o ) < eps )
	{
		theta_i = Clamp( theta_i, 0, M_PI );

		double cos_theta = cos( theta_i );

		LOOP_OVER_SPECTRUM ( i )
			value.attenuation_plain.spec[ i ] =
				Fresnel_Reflection_Plain(
					theta_i,
					1, 0,
					refractionIndex_n.spec[ i ],
					refractionIndex_k.spec[ i ] );

		pdf = 1;

		value = value / ( cos_theta );
	}
	else
	{
		LOOP_OVER_SPECTRUM ( i )
			value.attenuation_plain.spec[ i ] = 0;

		pdf = 0;
	}
}

void SpecularReflection::PrintBRDF ()
{
	printf( "Specular BRDF" );
}

Attenuation SpecularReflection::SampleBRDF (
	Vector in,
	Vector & out,
	Vector normal,
	double & pdf_out )
{
	Attenuation brdf = SampleBRDF( in, out, normal );
	pdf_out = PDF( in, normal, out );
	return brdf;
}

Attenuation SpecularReflection::SampleBRDF (
	Vector inWorld,
	Vector & outWorld,
	Vector normal
)
{
	GetSpecularReflectionDirection( inWorld, normal, outWorld );

	Attenuation brdfValue;
	BRDF( inWorld, outWorld, brdfValue );

	ASSERT_NAN( brdfValue );

	return brdfValue;
}

double SpecularReflection::PDF ( Vector in, Vector normal, Vector out )
{
	// ************** the out direction must be CALCULATED by specular direcion!! ********
	// otherwise we should return 0;
	// but here, PDF function are only queried in path tracer after getting specular outgoing direction
	// so we just return 1 for convinience.

	// this is actually conditional PDF(with outgoing direction sampled by fresnel reflection)
	return 1;


	// the following code should be used, but ... as stated above
	//		Vector calculated_nor = Normalize ( out - in );
	//
	//		if ( abs ( 1 - AbsDot ( calculated_nor , normal ) ) < eps)
	//			return 1;
	//		else return 0;
}

SpecularRefraction::SpecularRefraction ( Spectrum n1, Spectrum k1 )
{
	surfaceFlag = surface_delta;
	TIR = false;

	refractionIndex_n = n1;
	refractionIndex_k = k1;
}


// not implemented yet.....

// we use normal direction to determin 
// if the ray is inside of the object
void SpecularRefraction::BRDF (
	Vector in,
	Vector normal,
	Attenuation & value
)
{
	double cosi = Dot( in, normal );
	double theta = acos( abs( cosi ) );
	// normal direction always point to air
	// air -> media or media -> air 
	if ( cosi < 0 )
	{
		LOOP_OVER_SPECTRUM ( i )
			value.attenuation_plain.spec[ i ] =
				Fresnel_Refraction_plain
				( theta,
				  1, 0,
				  refractionIndex_n.spec[ i ],
				  refractionIndex_k.spec[ i ] );
	}
	if ( cosi > 0 )
	{
		LOOP_OVER_SPECTRUM ( i )
			value.attenuation_plain.spec[ i ] =
				Fresnel_Refraction_plain
				( theta,
				  refractionIndex_n.spec[ i ],
				  refractionIndex_k.spec[ i ],
				  1, 0 );
	}
	// it is actually not right
	// to be fixed

	ASSERT_NAN( value );

	//	if (value < eps || value > 1)
	//		printf ( "Error brdf value in Specular Refraction" );
}

// not implemented yet.....
void SpecularRefraction::BRDF (
	Vector in,
	Vector normal,
	Vector out,
	Attenuation & value,
	double & pdf )
{
	BRDF( in, normal, value );
	pdf = PDF( in, normal, out );
	printf( "Specular Refraction BRDF not implemented" );
}

// not implemented yet.....
Attenuation SpecularRefraction::SampleBRDF (
	Vector in,
	Vector & out,
	Vector normal,
	double & pdf_out )
{
	Attenuation brdf = SampleBRDF( in, out, normal );
	pdf_out = PDF( in, normal, out );
	return brdf;
}


// not implemented yet.....
Attenuation SpecularRefraction::SampleBRDF (
	Vector inWorld,
	Vector & outWorld,
	Vector normal
)
{
	// might choose reflection or refraction
	// because for transparent object, there are also some rays getting reflected 
	//	double cosi = Dot ( inWorld , normal );
	//	if (cosi < 0)
	//		GetSpecularRefractionDirection ( inWorld , normal , 1 , refractionIndex_n.x , TIR , outWorld );
	//	if (cosi > 0)
	//		GetSpecularRefractionDirection ( inWorld , normal , refractionIndex_n.x , 1 , TIR , outWorld );
	//	if (cosi == 0)
	//		outWorld = inWorld;
	// 
	Attenuation btdfValue;
	//	BRDF(inWorld, normal , btdfValue);

	if ( btdfValue.attenuation_plain.spec[ 0 ] > getUniformRandomNumber( 0.0, 1.0 ) && TIR == false )
		return btdfValue;
	else
	{
		//		GetSpecularReflectionDirection(inWorld, normal, outWorld);
		//		return Attenuation(1 - btdfValue.x, 1 - btdfValue.y, 1 - btdfValue.z);
	}
}

double SpecularRefraction::PDF ( Vector in, Vector normal, Vector out )
{
	// the same with SpecularReflection PDF()
	return 1;
}

void Lambertian::BRDF (
	Vector in,
	Vector out,
	Attenuation & value
)
{
	LOOP_OVER_SPECTRUM ( i )
		value.attenuation_plain.spec[ i ] = reflectance / ( M_PI );
}

void Lambertian::BRDF (
	Vector in,
	Vector normal,
	Vector out,
	Attenuation & value,
	double & pdf )
{
	BRDF( in, out, value );
	pdf = PDF( in, normal, out );
}

Attenuation Lambertian::SampleBRDF (
	Vector in,
	Vector & out,
	Vector normal,
	double & pdf_out )
{
	Attenuation brdf = SampleBRDF( in, out, normal );
	pdf_out = PDF( in, normal, out );
	return brdf;
}

Attenuation Lambertian::SampleBRDF (
	Vector inWorld,
	Vector & outWorld,
	Vector normal )
{
	TransformBetweenGlobalAndLocal trans;

	trans.InitTransformMatrix( inWorld, normal );

	double u1, u2;

	u1 = getUniformRandomNumber( 0.0, 1.0 );
	u2 = getUniformRandomNumber( 0.0, 1.0 );

	const double r = sqrt( 1.0 - u1 * u1 );
	const double phi = 2 * M_PI * u2;
	Vector outLocal = Vector( cos( phi ) * r, sin( phi ) * r, u1 );

	outWorld = trans.Local2World( outLocal );

	Attenuation brdfValue;
	BRDF( inWorld, outWorld, brdfValue );

	return brdfValue;
}

double Lambertian::PDF (
	Vector in,
	Vector normal,
	Vector out )
{
	double cos_theta = AbsDot( out, normal );
	return cos_theta / ( M_PI );
}

float Lambertian::getReflectance ( Vector in , Vector normal )
{
	return reflectance;
}

MeasuredBRDF::MeasuredBRDF (
	const char * filename,
	double roughness_parameter )
{
	uint32_t nThetaH, nThetaD, nPhiD;
	static map<string , KdTree<IrregIsotropicBRDFSample> *> loadedThetaPhi;

	const char * suffix = strrchr( filename, '.' );

	thetaPhiData = nullptr;

	if ( !suffix )
		printf( "No suffix in measured BRDF filename \"%s\".  "
		        "Can't determine file type (.brdf / .merl)", filename );
	else if ( !strcmp( suffix, ".brdf" ) || !strcmp( suffix, ".BRDF" ) )
	{
		// Load $(\theta, \phi)$ measured BRDF data
		if ( loadedThetaPhi.find( filename ) != loadedThetaPhi.end() )
		{
			thetaPhiData = loadedThetaPhi[ filename ];
			return;
		}

		vector<float> values;
		if ( !ReadFloatFile( filename, &values ) )
		{
			printf( "Unable to read BRDF data from file \"%s\"", filename );
			return;
		}

		uint32_t pos = 0;
		int numWls = int( values[ pos++ ] );
		if ( ( values.size() - 1 - numWls ) % ( 4 + numWls ) != 0 )
		{
			printf( "Excess or insufficient data in theta, phi BRDF file \"%s\"",
			        filename );
			return;
		}

		vector<float> wls;
		for ( int i = 0; i < numWls; ++i )
			wls.push_back( values[ pos++ ] );

		double rgb[3];

		vector<IrregIsotropicBRDFSample> samples;

		BBox bbox;
		while ( pos < values.size() )
		{
			float thetai = values[ pos++ ];
			float phii = values[ pos++ ];
			float thetao = values[ pos++ ];
			float phio = values[ pos++ ];
			Vector wo = SphericalDirection( sinf( thetao ), cosf( thetao ), phio );
			Vector wi = SphericalDirection( sinf( thetai ), cosf( thetai ), phii );
			//			Spectrum s = Spectrum::FromSampled ( &wls[ 0 ] , &values[ pos ] , numWls );

			rgb[ 0 ] = values[ pos++ ];
			rgb[ 1 ] = values[ pos++ ];
			rgb[ 2 ] = values[ pos++ ];

			Spectrum s( rgb );

			//			pos += numWls;
			Pnt3D p = BRDFRemap( wo, wi );
			samples.push_back( IrregIsotropicBRDFSample( p, s ) );
			bbox = Union( bbox, p );
		}
		thetaPhiData = new KdTree<IrregIsotropicBRDFSample>( samples );
	}

	roughness = roughness_parameter;
	surface = new Beckmann_Eric( roughness );
	surfaceFlag = surface_Measured;
}

void MeasuredBRDF::BRDF (
	Vector in,
	Vector out,
	Attenuation & value )
{
	Pnt3D m = BRDFRemap( out, in );
	double lastMaxDist2 = .001f;
	while ( true )
	{
		// Try to find enough BRDF samples around _m_ within search radius
		IrregIsoProc proc;
		double maxDist2 = lastMaxDist2;
		thetaPhiData->Lookup( m, proc, maxDist2 );
		if ( proc.nFound > 2 || lastMaxDist2 > 1.5f )
		{
			//	Vector d = Clamp ( proc.v , 0 , INFINITY );
			value.attenuation_plain = proc.v / proc.sumWeights;
			return;
		}

		lastMaxDist2 *= 2.f;
	}
}

void MeasuredBRDF::BRDF (
	Vector in,
	Vector normal,
	Vector out,
	Attenuation & value,
	double & pdf )
{
	BRDF( in, out, value );
	pdf = PDF( in, normal, out );
}


Attenuation MeasuredBRDF::SampleBRDF (
	Vector inWorld,
	Vector & outWorld,
	Vector normal )
{
	// initialise transformation matrix
	TransformBetweenGlobalAndLocal trans;
	trans.InitTransformMatrix( inWorld, normal );

	double u1, u2;

	u1 = getUniformRandomNumber( 0.0, 1.0 );
	u2 = getUniformRandomNumber( 0.0, 1.0 );

	const double r = sqrt( 1.0 - u1 * u1 );
	const double phi = 2 * M_PI * u2;
	Vector outLocal = Vector( cos( phi ) * r, sin( phi ) * r, u1 );

	outWorld = trans.Local2World( outLocal );

	Attenuation brdfValue;
	BRDF( inWorld, outWorld, brdfValue );

	return brdfValue;
}


// sample outWorld direction with given micro-surface
// and also return corresponding pdf
Attenuation MeasuredBRDF::SampleBRDF (
	Vector inWorld,
	Vector & outWorld,
	Vector normal,
	double & pdf_out )
{
	// initialise transformation matrix
	TransformBetweenGlobalAndLocal trans;
	trans.InitTransformMatrix( inWorld, normal );

	Vector local_in = trans.World2Local( inWorld );

	Vector local_normal;
	local_normal = surface->sample_wh( local_in, Vector( 0, 0, 1 ) );

	GetSpecularReflectionDirection( local_in, local_normal, outWorld );

	Attenuation brdfValue;
	BRDF( local_in, Vector( 0, 0, 1 ), outWorld, brdfValue, pdf_out );

	outWorld = trans.Local2World( outWorld );

	return brdfValue;
}

double MeasuredBRDF::PDF ( Vector in, Vector normal, Vector out )
{
	double pdf_out;
	Vector local_normal = Normalize( out - in );
	double cos_theta = AbsDot( in, local_normal );

	double D = surface->D_wh( &in, &local_normal, &normal, pdf_out );

	// turn pdf of local normal to pdf of sampled outgoing direction
	pdf_out = pdf_out / ( 4 * cos_theta );

	return pdf_out;
}

