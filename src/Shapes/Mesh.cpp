#include "Mesh.h"

using namespace tinyply;

void Mesh::LoadObjFile ()
{
	const char * file_path;

	std::string inputfile = "cornell_box.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj( &attrib, &shapes, &materials, &err, inputfile.c_str() );

	if ( !err.empty() )
	{ // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if ( !ret )
	{
		exit( 1 );
	}

	// Loop over shapes
	for ( size_t s = 0; s < shapes.size(); s++ )
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;

		for ( size_t f = 0; f < shapes[ s ].mesh.num_face_vertices.size(); f++ )
		{
			int fv = shapes[ s ].mesh.num_face_vertices[ f ];

			// Loop over vertices in the face.
			for ( size_t v = 0; v < fv; v++ )
			{
				// access to vertex
				tinyobj::index_t idx = shapes[ s ].mesh.indices[ index_offset + v ];
				float vx = attrib.vertices[ 3 * idx.vertex_index + 0 ];
				float vy = attrib.vertices[ 3 * idx.vertex_index + 1 ];
				float vz = attrib.vertices[ 3 * idx.vertex_index + 2 ];
				//				float nx = attrib.normals[ 3 * idx.normal_index + 0 ];
				//				float ny = attrib.normals[ 3 * idx.normal_index + 1 ];
				//				float nz = attrib.normals[ 3 * idx.normal_index + 2 ];
				//				float tx = attrib.texcoords[ 2 * idx.texcoord_index + 0 ];
				//				float ty = attrib.texcoords[ 2 * idx.texcoord_index + 1 ];
			}
			index_offset += fv;

			// per-face material
			shapes[ s ].mesh.material_ids[ f ];
		}
	}
}

// Only read ply file in binary encoding!!!
void Mesh::tinyPLY ( const std::string & filename )
{
	// Tinyply can and will throw exceptions at you!
	try
	{
		// Read the file and create a std::istringstream suitable
		// for the lib -- tinyply does not perform any file i/o.
		std::ifstream ss( filename, std::ios::binary );

		// Parse the ASCII header fields
		PlyFile file( ss );

		for ( auto e : file.get_elements() )
		{
			std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
			for ( auto p : e.properties )
			{
				std::cout << "\tproperty - " << p.name << " (" << PropertyTable[ p.propertyType ].str << ")" << std::endl;
			}
		}
		std::cout << std::endl;
		string texture_file;

		for ( auto c : file.comments )
		{
			std::cout << "Comment: " << c << std::endl;
			int pos = c.find ( "TextureFile" );
			int len = c.size ();
			if (pos != -1)
				texture_file = c.substr ( 12 , len - 12 );
		}

		// Define containers to hold the extracted data. The type must match
		// the property type given in the header. Tinyply will interally allocate the
		// the appropriate amount of memory.
		std::vector<float> verts;
		std::vector<float> norms;
		std::vector<uint8_t> colors;

		std::vector<uint32_t> faces;
		std::vector<float> uvCoords;

		uint32_t vertexCount, normalCount, colorCount, faceCount, faceTexcoordCount, faceColorCount;
		vertexCount = normalCount = colorCount = faceCount = faceTexcoordCount = faceColorCount = 0;

		// The count returns the number of instances of the property group. The vectors
		// above will be resized into a multiple of the property group size as
		// they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
		vertexCount = file.request_properties_from_element( "vertex", { "x", "y", "z" }, verts );
		normalCount = file.request_properties_from_element( "vertex", { "nx", "ny", "nz" }, norms );
		colorCount = file.request_properties_from_element( "vertex", { "red", "green", "blue", "alpha" }, colors );

		// For properties that are list types, it is possibly to specify the expected count (ideal if a
		// consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
		// defers allocation of memory until the first instance of the property has been found
		// as implemented in file.read(ss)
		faceCount = file.request_properties_from_element( "face", { "vertex_indices" }, faces, 3 );
		faceTexcoordCount = file.request_properties_from_element( "face", { "texcoord" }, uvCoords, 6 );

		// Now populate the vectors...
		//		timepoint before = now ();
		file.read( ss );
		//		timepoint after = now ();

		// Good place to put a breakpoint!
		//		std::cout << "Parsing took " << difference_micros ( before , after ) << "¦Ìs: " << std::endl;
		std::cout << "\tRead " << verts.size() << " total vertices (" << vertexCount << " properties)." << std::endl;
		std::cout << "\tRead " << norms.size() << " total normals (" << normalCount << " properties)." << std::endl;
		std::cout << "\tRead " << colors.size() << " total vertex colors (" << colorCount << " properties)." << std::endl;
		std::cout << "\tRead " << faces.size() << " total faces (triangles) (" << faceCount << " properties)." << std::endl;
		std::cout << "\tRead " << uvCoords.size() << " total texcoords (" << faceTexcoordCount << " properties)." << std::endl;

		/*
		// Fixme - tinyply isn't particularly sensitive to mismatched properties and prefers to crash instead of throw. Use
		// actual data from parsed headers instead of manually defining properties added to a new file like below:

		std::filebuf fb;
		fb.open("converted.ply", std::ios::out | std::ios::binary);
		std::ostream outputStream(&fb);

		PlyFile myFile;

		myFile.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
		myFile.add_properties_to_element("vertex", { "red", "green", "blue" }, colors);
		myFile.add_properties_to_element("face", { "vertex_indices" }, faces, 3, PlyProperty::Type::UINT8);

		myFile.comments.push_back("generated by tinyply");
		myFile.write(outputStream, true);

		fb.close();
		*/

		ResetMemory( faceCount, vertexCount );

		Vertex * ver_array;
		Triangle_index_bre * tri_array;

		ver_array = ( Vertex * ) malloc( num_of_vertex * sizeof ( Vertex) );
		tri_array = ( Triangle_index_bre * ) malloc( num_of_triangle * sizeof ( Triangle_index_bre) );

		// read vector into Vertex and Triangle_index_bre
		for ( int i = 0; i < faceCount; i++ )
		{
			tri_array[ i ].v0 = faces[ i * 3 ];
			tri_array[ i ].v1 = faces[ i * 3 + 1 ];
			tri_array[ i ].v2 = faces[ i * 3 + 2 ];
		}

		for ( int i = 0; i < vertexCount; i++ )
		{
			ver_array[ i ].x = verts[ i * 3 ];
			ver_array[ i ].y = verts[ i * 3 + 1 ];
			ver_array[ i ].z = verts[ i * 3 + 2 ];
		}

		SetMesh( ver_array, tri_array );

		delete ver_array;
		delete tri_array;

		verts.clear();
		faces.clear();


		if (uvCoords.size () > 0 && texture_file != "")
		{
			texture.InitUVMap ( uvCoords );
			texture.LoadTextureFile ( texture_file );
		}

	}

	catch ( const std::exception & e )
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}
	
}


Mesh::~Mesh ()
{
}

