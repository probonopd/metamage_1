/*
	clock.cc
	--------
	
	Copyright 2019 Joshua Juran.  Released under the GPL 3.0 (or later).
*/

#include "clock.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Standard C
#include <time.h>


const int us_per_tick = 16625;
const int ns_per_tick = 16625104;

#ifdef CLOCK_MONOTONIC

unsigned int uptime_units_per_tick = ns_per_tick;  // clock_gettime()

#else

unsigned int uptime_units_per_tick = us_per_tick;  // Microseconds()

#endif

unsigned long long uptime()
{
#ifdef CLOCK_MONOTONIC
	
	timespec ts;
	
	int nok = clock_gettime( CLOCK_MONOTONIC, &ts );
	
	return + ts.tv_sec * 1000000000ull
	       + ts.tv_nsec;
	
#else
	
	unsigned long long result;
	
	Microseconds( (UnsignedWide*) &result );
	
	return result;
	
#endif
}
