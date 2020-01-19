/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:15:16
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:15:16
 * @Description: All rights reserved
 */

#include "Foundations/Vector.h"
#include "Core/Ray.h"
#include "Foundations/RandomNumber.h"

#pragma once
class Camera
{
public:
	size_t width;
	size_t height;

	// eye position and image center position
	Pnt3D eyeOrigin;
	Pnt3D imageOrigin;

	Vector lookAtDirection;
	Vector upDirection; // up direction
	Vector thirdDirection; // horizental direction of camera ( point to the right)

	double focalLength;
public:

	Camera (
		size_t w,
		size_t h,
		Pnt3D ep,
		Pnt3D imagep,
		Vector upDir
	);
	Camera ();
	~Camera ();

	virtual void CastingRayFromCamera (
		size_t x,
		size_t y,
		Ray & ray
	)
	{
	}
};


class PinholeCamera : public Camera
{
	double fovx;
	double fovy;
	double shiftDistanceX;
	double shiftDistanceY;
public:
	PinholeCamera (
		size_t w,
		size_t h,
		double fov_x,
		double fov_y,
		Pnt3D ep,
		Pnt3D ip,
		Vector upDir
	);
	PinholeCamera ();

	void CastingRayFromCamera (
		size_t x,
		size_t y,
		Ray & ray
	);
};

