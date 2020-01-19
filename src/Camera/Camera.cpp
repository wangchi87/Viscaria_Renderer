#include "Camera.h"


Camera::Camera (
	size_t w,
	size_t h,
	Vector ep,
	Vector imagep,
	Vector upDir )
{
	width = w;
	height = h;

	eyeOrigin = ep;
	imageOrigin = imagep;

	lookAtDirection = imageOrigin - eyeOrigin;
	focalLength = length( lookAtDirection );

	lookAtDirection = Normalize( lookAtDirection );
	upDirection = upDir;
	
	//right hand coord system����������ϵ��
	//thirdDirection = Cross( upDirection, lookAtDirection );

	//left hand coord system����������ϵ��
	thirdDirection = Cross( lookAtDirection, upDirection );


	upDirection = Normalize( upDirection );
	thirdDirection = Normalize( thirdDirection );
}

Camera::Camera ()
{
}


Camera::~Camera ()
{
}

PinholeCamera::PinholeCamera (
	size_t w,
	size_t h,
	double fov_x,
	double fov_y,
	Vector ep,
	Vector ip,
	Vector upDir )
	: Camera( w, h, ep, ip, upDir )
{
	fovx = fov_x;
	fovy = fov_y;

	//minimum shift unit along x and y direction of the image plane 
	shiftDistanceX = ( 2 * focalLength * tan( fovx / 2 ) ) / width;
	shiftDistanceY = ( 2 * focalLength * tan( fovy / 2 ) ) / height;
}

PinholeCamera::PinholeCamera ()
{
}

void PinholeCamera::CastingRayFromCamera (
	size_t x,
	size_t y,
	Ray & ray )
{
	double w_div_2 = width / 2;
	double h_div_2 = height / 2;

	double xx = getUniformRandomNumber( 0.0, 1.0 );
	double yy = getUniformRandomNumber( 0.0, 1.0 );

	xx += x;
	yy += y;

	double alpha = shiftDistanceX * ( ( xx - w_div_2 ) );
	double beta = shiftDistanceY * ( ( h_div_2 - yy ) );

	Vector rayDir = lookAtDirection +
		alpha * thirdDirection +
		beta * upDirection;

	rayDir = Normalize( rayDir );

	ray = Ray( eyeOrigin, rayDir );
}

