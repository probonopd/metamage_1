/*
	stdexcept.hh
	------------
*/

#include "plus/stdexcept.hh"


namespace plus
{
	
	void out_of_range( const char* message )
	{
		throw std::out_of_range( std::string( message ) );
	}
	
}

