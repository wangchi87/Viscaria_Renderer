#pragma once
#include <stdlib.h>

// integrated from PBRT
// not be used currently

typedef unsigned __int32 uint32_t;

template< typename T, int logBlockSize >
class BlockedArray
{
public:
	// BlockedArray Public Methods
	BlockedArray ( size_t nu, size_t nv, const T * d = NULL )
	{
		uRes = nu;
		vRes = nv;
		uBlocks = RoundUp( uRes ) >> logBlockSize;
		size_t nAlloc = RoundUp( uRes ) * RoundUp( vRes );
		data = AllocAligned<T>( nAlloc );
		for ( size_t i = 0; i < nAlloc; ++i )
			new( &data[ i ] ) T();
		if ( d )
			for ( size_t v = 0; v < vRes; ++v )
				for ( size_t u = 0; u < uRes; ++u )
					( *this )( u, v ) = d[ v * uRes + u ];
	}

	size_t BlockSize () const
	{
		return 1 << logBlockSize;
	}

	size_t RoundUp ( size_t x ) const
	{
		return ( x + BlockSize() - 1 ) & ~( BlockSize() - 1 );
	}

	size_t uSize () const
	{
		return uRes;
	}

	size_t vSize () const
	{
		return vRes;
	}

	~BlockedArray ()
	{
		for ( size_t i = 0; i < uRes * vRes; ++i )
			data[ i ].~T();
		FreeAligned( data );
	}

	size_t Block ( size_t a ) const
	{
		return a >> logBlockSize;
	}

	size_t Offset ( size_t a ) const
	{
		return ( a & ( BlockSize() - 1 ) );
	}

	T & operator() ( size_t u, size_t v )
	{
		size_t bu = Block( u ), bv = Block( v );
		size_t ou = Offset( u ), ov = Offset( v );
		size_t offset = BlockSize() * BlockSize() * ( uBlocks * bv + bu );
		offset += BlockSize() * ov + ou;
		return data[ offset ];
	}

	const T & operator() ( size_t u, size_t v ) const
	{
		size_t bu = Block( u ), bv = Block( v );
		size_t ou = Offset( u ), ov = Offset( v );
		size_t offset = BlockSize() * BlockSize() * ( uBlocks * bv + bu );
		offset += BlockSize() * ov + ou;
		return data[ offset ];
	}

	void GetLinearArray ( T * a ) const
	{
		for ( size_t v = 0; v < vRes; ++v )
			for ( size_t u = 0; u < uRes; ++u )
				*a++ = ( *this )( u, v );
	}

private:
	// BlockedArray Private Data
	T * data;
	size_t uRes, vRes, uBlocks;
};


void * AllocAligned ( size_t size );

template< typename T >
T * AllocAligned ( uint32_t count )
{
	return ( T * ) AllocAligned( count * sizeof ( T) );
}


void FreeAligned ( void * );

