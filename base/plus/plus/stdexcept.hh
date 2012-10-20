/*
	stdexcept.hh
	------------
*/

#ifndef PLUS_STDEXCEPT_HH
#define PLUS_STDEXCEPT_HH

// Standard C++
#include <stdexcept>


namespace plus
{
	
	void length_error( const char* message );
	
	void out_of_range( const char* message );
	
}

#endif

