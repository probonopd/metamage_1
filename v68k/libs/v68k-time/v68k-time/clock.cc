/*
	clock.cc
	--------
*/

#include "v68k-time/clock.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifdef __MACOS__
#ifndef __TIMER__
#include <Timer.h>
#endif
#endif

// Standard C
#include <time.h>


#pragma exceptions off


#ifdef __RELIX__
#undef CLOCK_MONOTONIC  // It's defined, but clock_gettime() isn't
#endif


namespace v68k {
namespace time {

static
uint64_t host_uptime_microseconds()
{
#ifdef CLOCK_MONOTONIC
	
	timespec ts;
	
	int nok = clock_gettime( CLOCK_MONOTONIC, &ts );
	
	return + ts.tv_sec * 1000000ull
	       + ts.tv_nsec / 1000;
	
#else
	
	uint64_t result;
	
	Microseconds( (UnsignedWide*) &result );
	
	return result;
	
#endif
}

static const uint64_t origin = host_uptime_microseconds();

uint64_t guest_uptime_microseconds()
{
	return host_uptime_microseconds() - origin;
}

}  // namespace time
}  // namespace v68k
