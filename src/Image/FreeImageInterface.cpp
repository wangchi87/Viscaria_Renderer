#include "FreeImageInterface.h"


unsigned DLL_CALLCONV myReadProc ( void *buffer , unsigned size , unsigned count , fi_handle handle ) {
	return fread ( buffer , size , count , ( FILE * ) handle );
}

unsigned DLL_CALLCONV myWriteProc ( void *buffer , unsigned size , unsigned count , fi_handle handle ) {
	return fwrite ( buffer , size , count , ( FILE * ) handle );
}

int DLL_CALLCONV mySeekProc ( fi_handle handle , long offset , int origin ) {
	return fseek ( ( FILE * ) handle , offset , origin );
}

long DLL_CALLCONV myTellProc ( fi_handle handle ) {
	return ftell ( ( FILE * ) handle );
}

void FreeImageErrorHandler ( FREE_IMAGE_FORMAT fif , const char *message ) {
	printf ( "\n*** " );
	if (fif != FIF_UNKNOWN) {
		printf ( "%s Format\n" , FreeImage_GetFormatFromFIF ( fif ) );
	}
	printf ( message );
	printf ( " ***\n" );
}

FreeImageInterface::FreeImageInterface ()
{
	FreeImage_SetOutputMessage ( FreeImageErrorHandler );

	io.read_proc = myReadProc;
	io.write_proc = myWriteProc;
	io.seek_proc = mySeekProc;
	io.tell_proc = myTellProc;
}


FreeImageInterface::~FreeImageInterface ()
{
	// free the loaded FIBITMAP
	FreeImage_Unload ( dib );
}

void FreeImageInterface::LoadImage ( const std::string & filename )
{
	
	// find the buffer format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename ( filename.c_str () );

	if (fif != FIF_UNKNOWN) 
	{
		// load from the file handle
		dib = FreeImage_Load ( fif , filename.c_str () );
		height = FreeImage_GetHeight ( dib );
		width = FreeImage_GetWidth ( dib );
	}

	ApplyGammaCorrection ( 1 / 2.2 );
}

void FreeImageInterface::SaveImage ( const std::string & filename )
{

	// first, check the output format from the file name or file extension
	FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename ( filename.c_str() );

	if (out_fif != FIF_UNKNOWN) {
		// then save the file
		FreeImage_Save ( out_fif , dib , filename.c_str () , 0 );
	}

	//const unsigned w = 320;
	//const unsigned h = 240;

	//FIBITMAP *bmp = FreeImage_Allocate ( w , h , 24 , 0 , 0 , 0 );

	//RGBQUAD color = { 0xff, 0xff, 0x00, 0x00 };

	//unsigned x , y;

	//for (y = 0; y < h / 2; y++) // filling a half of the image
	//{
	//	for (x = 0; x < w / 2; x++) // the X coordinate seems to be fine
	//	{
	//		FreeImage_SetPixelColor ( bmp , x , y , &color );
	//	}
	//}
	//FreeImage_Save ( FIF_BMP , bmp , "test_out.bmp" , 0 );
	//FreeImage_Unload ( bmp );
}

void FreeImageInterface::GetPixelValue ( unsigned int u , unsigned int v,  double &r, double &g, double &b)
{

	if (u <= width && v <= height)
	{
		RGBQUAD color;
		//Be aware that in FreeImage the origin is bottom left , 
		//so y values will probably need to be inverted by subtracting y from the image height as above
		bool state = FreeImage_GetPixelColor ( dib , u , v , &color );
	
		r = color.rgbRed;
		g = color.rgbGreen;
		b = color.rgbBlue;
	}
	else
	{
		printf ( "u v coord is out of bound" );
	}
}

void FreeImageInterface::WritePixelValue ( unsigned int u , unsigned int v , double r , double g , double b )
{
	if (u <= width && v <= height)
	{
		RGBQUAD color;

		color.rgbRed = r;
		color.rgbGreen = g;
		color.rgbBlue = b;

		//Be aware that in FreeImage the origin is bottom left , 
		//so y values will probably need to be inverted by subtracting y from the image height as above
		bool state2 = FreeImage_SetPixelColor ( dib , u , height - 1- v , &color );

	}
	else
	{
		printf ( "u v coord is out of bound" );
	}
}

void FreeImageInterface::ApplyGammaCorrection ( float gamma )
{
	FreeImage_AdjustGamma ( dib , gamma );
}

void FreeImageInterface::ToneMappingDrago ( float gamma , float exposure )
{
	dib = FreeImage_TmoDrago03 ( dib , gamma , exposure );
}

void FreeImageInterface::AllocateMemory ( size_t width , size_t height )
{
	this->width = width;
	this->height = height;
	dib = FreeImage_Allocate ( width , height, 24,  0 , 0 , 0 );

	if (!dib)
		printf ( "failed to allocate memory for freeimage" );
}

size_t FreeImageInterface::GetWidth ()
{

	return width;
}

size_t FreeImageInterface::GetHeight ()
{
	return height;
}


