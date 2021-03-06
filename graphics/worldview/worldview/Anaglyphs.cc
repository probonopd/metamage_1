/*
	worldview/Anaglyphs.cc
	----------------------
*/

#include "worldview/Anaglyphs.hh"

// iota
#include "iota/endian.hh"


namespace worldview
{
	
	typedef unsigned char byte;
	
	enum
	{
		A_BE = 0,
		R_BE = 1,
		G_BE = 2,
		B_BE = 3,
		
		B_LE = 0,
		G_LE = 1,
		R_LE = 2,
		A_LE = 3,
	};
	
	const int A = iota::is_little_endian() ? A_LE : A_BE;
	const int R = iota::is_little_endian() ? R_LE : R_BE;
	const int G = iota::is_little_endian() ? G_LE : G_BE;
	const int B = iota::is_little_endian() ? B_LE : B_BE;
	
	static
	void MergeTrueAnaglyph( const byte* left, byte* right )
	{
		double r = 0.299 * left  [R] + 0.587 * left  [G] + 0.114 * left  [B];
		double g = 0;
		double b = 0.299 * right [R] + 0.587 * right [G] + 0.114 * right [B];
		
		right[ R ] = (byte) r;
		right[ G ] = (byte) g;
		right[ B ] = (byte) b;
	}
	
	static
	void MergeGrayAnaglyph( const byte* left, byte* right )
	{
		double r = 0.299 * left  [R] + 0.587 * left  [G] + 0.114 * left  [B];
		double g = 0.299 * right [R] + 0.587 * right [G] + 0.114 * right [B];
		double b = g;
		
		right[ R ] = (byte) r;
		right[ G ] = (byte) g;
		right[ B ] = (byte) b;
	}
	
	static
	void MergeColorAnaglyph( const byte* left, byte* right )
	{
		right[R] = left[R];
	}
	
	static
	void MergeHalfColorAnaglyph( const byte* left, byte* right )
	{
		right[R] = byte( 0.299 * left[R] + 0.587 * left[G] + 0.114 * left[B] );
	}
	
	static
	void MergeOptimizedAnaglyph( const byte* left, byte* right )
	{
		right[R] = byte( 0.7 * left[G] + 0.3 * left[B] );
	}
	
	typedef void (*anaglyphic_merge_proc)( const byte*, byte* );
	
	static
	anaglyphic_merge_proc select_anaglyphic_merge( anaglyph_mode mode )
	{
		anaglyphic_merge_proc merge;
		
		switch ( mode )
		{
			case kTrueAnaglyph:
				merge = &MergeTrueAnaglyph;
				break;
			
			case kGrayAnaglyph:
				merge = &MergeGrayAnaglyph;
				break;
			
			case kColorAnaglyph:
				merge = &MergeColorAnaglyph;
				break;
			
			case kHalfColorAnaglyph:
				merge = &MergeHalfColorAnaglyph;
				break;
			
			case kOptimizedAnaglyph:
				merge = &MergeOptimizedAnaglyph;
				break;
			
			default:
				merge = NULL;
		}
		
		return merge;
	}
	
	void merge_anaglyph( anaglyph_mode  mode,
	                     void const*    left,
	                     void*          right,
	                     size_t         height,
	                     size_t         width,
	                     size_t         stride )
	{
		anaglyphic_merge_proc merge = select_anaglyphic_merge( mode );
		
		if ( merge == NULL )
		{
			return;
		}
		
		const int pixel_size = 4;
		
		size_t gutter = stride - width * pixel_size;
		
		byte const* a = (byte const*) left;
		byte*       b = (byte*)       right;
		
		for ( unsigned y = 0;  y < height;  ++y )
		{
			for ( unsigned x = 0;  x < width;  ++x )
			{
				merge( a, b );
				
				a += pixel_size;
				b += pixel_size;
			}
			
			a += gutter;
			b += gutter;
		}
	}
	
}
