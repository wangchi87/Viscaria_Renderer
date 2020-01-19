/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:19:55
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:19:55
 * @Description: All rights reserved
 */
#include <iostream>
#include <fstream>

#include <LightAndAttenuation/BRDFModels.h>
#include <LightAndAttenuation/MicroFacetModel.h>

using namespace std;

int main ( int argc , char * argv [] )
{
	cout << "This is BRDF evaluator for Chi_render." << endl;

	
	char filename [] = "BRDF_Data_Die_15_r2.txt";

	ofstream ofs ( filename );

	int thetai = 29;

	int thetas_1 = 0;
	int thetas_2 = 90;
	int thetas_3 = 1;

	int phis_1 = 0;
	int phis_2 = 360;
	int phis_3 = 1;

	Vector in_vector;

	in_vector = - SphericalDirection (
		thetai * MATH_DEG_TO_RAD ,
		180 * MATH_DEG_TO_RAD );

	in_vector.x = -0.606907;
	in_vector.y = -0.316805;
	in_vector.z =  0.728902;

	Vector out_vector;

	double nn[ 3 ] = { 1.5,1.5,1.5 };
	double kk[ 3 ] = { 0,0,0 };

	Spectrum n ( nn );
	Spectrum k ( kk );

	Vector normal ( 0.5 , 0.5 , -0.707107 );
	double pdf = 0;

	BRDFModels * brdf;
	
	brdf = new TorranceSparrow ( false, 0.25 , n , k );

//	brdf = new Lambertian ( 0.9 );

//	brdf = new SpecularReflection ( n , k );

	double albedo = 0;

	double int_pdf = 0;

	for (int thetas = thetas_1; thetas <= thetas_2; thetas += thetas_3)
	{
		for (int phis = phis_1; phis <= phis_2; phis += phis_3)
		{
			out_vector = SphericalDirection (
				thetas * MATH_DEG_TO_RAD ,
				phis * MATH_DEG_TO_RAD );
//
			out_vector.x = -0.144522;
			out_vector.x = -0.771323;
			out_vector.x =  0.619818;


			Attenuation brdf_value ;
//
			brdf->BRDF (
				in_vector ,
				normal,
				out_vector ,
				brdf_value,
				pdf);

			
			double brdf = brdf_value.attenuation_plain[ 0 ];

			double dw = abs ( sin ( thetas * MATH_DEG_TO_RAD ) )* MATH_DEG_TO_RAD * MATH_DEG_TO_RAD;
			
			double brdf_cos = brdf * abs ( out_vector.z );

			if(brdf_cos>1)
			{
				printf ( "%f" , brdf_cos );
			}

//			albedo += brdf_cos;
			albedo += brdf_cos *dw ;
			int_pdf += pdf *dw;

			ofs << out_vector.x << " "   // Scattering angle (theta)
				<< out_vector.y << " "
				<< out_vector.z << " "
				<< in_vector.x << " "
				<< in_vector.y << " "
				<< in_vector.z << " "
				<< brdf
				<< std::endl;
		}
	}
	cout << "int_pdf: " << int_pdf << endl;
	cout << "albedo : " << albedo << endl;

	return 0;
}