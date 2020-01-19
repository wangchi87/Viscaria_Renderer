# Viscaria_Renderer

This is a small renderer developed and maintained by Chi Wang(http://cgg.mff.cuni.cz/~chi/).

The renderer is named after one of my favourite table tennis blade "Viscaria" from Butterfly. But, viscaria, itself, is also a beautiful flower.

The features of the renderer: 

1. implement path tracing with and without multiple importance sampling, as well as direct illumination;

2. include Microfacet BRDF models on GGX surface, and on Beckmann surface(with visible normal distribution sampling), as well as Lambertian, specular reflection BRDF models; 

3. intersection test accelerated by Embree;

4. multi-threaded by OpenMP;

5. support reading PLY geometry file;

6. POLARISATION ray tracing supports are on the way.

*****************************************************************

Executable can be found in Gallery folder.

Run the renderer with scene file as following in command line:

Viscaria.exe happy_new_year.txt


*****************************************************************
To run the executable elsewhere in Windows, it is necessary to add embree lib folder(.....\Viscaria_Renderer\embree\lib) to system PATH variable.

*****************************************************************
To re-compile the project, what you need to do is just opening ".sln" file directly in Visual Studio 2015.
