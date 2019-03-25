/*
	macsnddrvrsynth.cc
	------------------
*/

// POSIX
#include <unistd.h>
#include <sys/time.h>

// Standard C
#include <stdlib.h>
#include <string.h>
#include <time.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// macsnddrvrsynth
#include "clock.hh"
#include "command-queue.hh"
#include "four-tone.hh"
#include "free-form.hh"
#include "options.hh"
#include "square-wave.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "macsnddrvrsynth"

#define USAGE "Usage: " PROGRAM " [--in-fd] [--out-fd] [--stream] [--verbose]\n"

#define WARN( msg )  if ( verbosity >= 1 ) write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) ); else


static const uint8_t silence = 0x80;

enum
{
	Opt_in_fd   = 'i',
	Opt_out_fd  = 'o',
	Opt_verbose = 'v',
	
	Opt_last_byte = 255,
	
	Opt_stream,
};

static command::option options[] =
{
	{ "in-fd", Opt_in_fd, command::Param_required },
	{ "out-fd", Opt_out_fd, command::Param_required },
	{ "stream", Opt_stream, command::Param_optional },
	{ "verbose", Opt_verbose, command::Param_required },
	
	NULL,
};

static
void queue_pending_commands()
{
	switch ( read_fd_ready( in_fd ) )
	{
		case -1:
			WARN( "read_fd_ready() returned -1, gracefully exit" );
			break;
		
		case 0:
			// nothing to read
			break;
		
		default:
			read_into_queue( in_fd );
			break;
	}
}

static
void synthesize_tick()
{
	const int sample_buffer_size = 370;
	uint8_t output_buffer[ sample_buffer_size ];
	size_t sample_index = 0;
	size_t samples_synthesized = 0;
	
	sound_buffer* buffer = peek_queue_front();
	
	if ( buffer != 0 )
	{
		switch ( buffer->mode )
		{
			case swMode:
				samples_synthesized = synthesize_square_wave_samples( buffer->square_wave, &output_buffer[ sample_index ], sample_buffer_size );
				break;
			
			case ftMode:
				samples_synthesized = synthesize_four_tone_samples( buffer->four_tone, &output_buffer[ sample_index ], sample_buffer_size );
				break;
			
			case ffMode:
				samples_synthesized = synthesize_free_form_samples( buffer->free_form, &output_buffer[ sample_index ], sample_buffer_size );
				break;
			
			default:
				WARN( "unsupported synthesizer mode" );
				break;
		}
		
		if ( samples_synthesized == 0 )
		{
			pop_from_queue();
		}
	}
	
	if ( stream )
	{
		if ( samples_synthesized < sample_buffer_size )
		{
			memset( &output_buffer[ samples_synthesized ], silence, sample_buffer_size - samples_synthesized );
		}
		
		write( out_fd, output_buffer, sample_buffer_size );
	}
	else if ( samples_synthesized > 0 )
	{
		write( out_fd, output_buffer, samples_synthesized );
	}
}

static
char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options ) ) > 0 )
	{
		switch ( opt )
		{
			case Opt_in_fd:
				in_fd = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_out_fd:
				out_fd = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_stream:
				if ( global_result.param == NULL || strcmp( global_result.param, "true" ) == 0 )
				{
					stream = true;
				}
				else if ( strcmp( global_result.param, "false" ) == 0 )
				{
					stream = false;
				}
				break;
			
			case Opt_verbose:
				verbosity = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

static const int us_per_tick = 16625;

static
void event_loop()
{
	uint64_t then = uptime();
	
	while ( true )
	{
		uint64_t now = uptime();
		
		uint64_t elapsed = now - then;
		unsigned elapsed_ticks = elapsed / uptime_units_per_tick;
		
		queue_pending_commands();
		
		if ( elapsed_ticks > 0 )
		{
			unsigned ticks_left = elapsed_ticks;
			
			do
			{
				synthesize_tick();
				--ticks_left;
			}
			while ( ticks_left > 0 );
			
			then += elapsed_ticks * uptime_units_per_tick;
		}
		
		timespec frame_time = { 0, us_per_tick * 1000 / 2 };
		nanosleep( &frame_time, NULL );
	}
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	event_loop();
	
	return 0;
}
