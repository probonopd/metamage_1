/*
	stdexcept.hh
	------------
*/

#include "plus/stdexcept.hh"

// Standard C++
#include <string>


namespace plus
{
	
	void out_of_range( const char* message )
	{
		throw std::out_of_range( std::string( message ) );
	}
	
}

