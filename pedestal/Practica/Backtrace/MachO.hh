/*	========
 *	MachO.hh
 *	========
 */

#ifndef BACKTRACE_MACHO_HH
#define BACKTRACE_MACHO_HH

// Standard C++
#include <string>

// Backtrace
#include "Backtrace/StackCrawl.hh"


namespace Backtrace
{
	
	const char* FindSymbolName( ReturnAddrPPCMachO addr );
	
	inline std::string GetSymbolString( const char* name )
	{
		return name;
	}
	
}

#endif

