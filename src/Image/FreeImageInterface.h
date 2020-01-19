/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:23
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:23
 * @Description: All rights reserved
 */
#pragma once
#include<string>
#include <FreeImage.h>


class FreeImageInterface
{
	FreeImageIO io;
	FIBITMAP *dib;
	size_t width;
	size_t height;

public:
	FreeImageInterface ();
	~FreeImageInterface ();

	void LoadImage ( const std::string & filename );

	void SaveImage ( const std::string & filename );

	void GetPixelValue ( unsigned int u , unsigned int v , double &r , double &g , double &b );
	void WritePixelValue ( unsigned int u , unsigned int v , double r , double g , double b );

	void ApplyGammaCorrection ( float gamma );

	void ToneMappingDrago ( float gamma , float exposure );

	void AllocateMemory ( size_t width, size_t height);

	size_t GetWidth ();
	size_t GetHeight ();

};

