/*
	command-queue.cc
	----------------
*/

#include "command-queue.hh"

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Standard C
#include <stdlib.h>

// iota
#include "iota/endian.hh"

// macsnddrvrsynth
#include "options.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "macsnddrvrsynth"

#define WARN( msg )  if ( verbosity >= 1 ) write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) ); else


enum
{
	basic_domain = 0x0101,
	sound_domain = 0x4B4B,
};

struct basic_buffer
{
	uint16_t code;
};

struct queueable_command
{
	sound_buffer buffer;
	queueable_command* next;
};

queueable_command* command_queue_head = NULL;
queueable_command* command_queue_tail = NULL;

static
sound_buffer& add_to_queue()
{
	queueable_command* new_command = (queueable_command*) malloc( sizeof *new_command );
	new_command->next = NULL;
	
	if ( command_queue_head == NULL )
	{
		command_queue_head = new_command;
	}
	else
	{
		command_queue_tail->next = new_command;
	}
	
	command_queue_tail = new_command;
	
	return new_command->buffer;
}

sound_buffer* peek_queue_front()
{
	if ( command_queue_head != 0 )
	{
		return &command_queue_head->buffer;
	}
	else
	{
		return 0;
	}
}

void pop_from_queue()
{
	if ( command_queue_head != 0 )
	{
		queueable_command* new_head = command_queue_head->next;
		free( command_queue_head );
		command_queue_head = new_head;
		if ( command_queue_head == NULL )
		{
			command_queue_tail = NULL;
		}
	}
}

void clear_queue()
{
	while ( peek_queue_front() != NULL )
	{
		pop_from_queue();
	}
}

int read_fd_ready( int fd )
{
	fd_set readfds;
	FD_ZERO( &readfds );
	FD_SET( fd, &readfds );
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	return select( fd + 1, &readfds, 0, 0, &tv );
}

static
ssize_t read_command_header( int fd, uint16_t* domain, uint32_t* length )
{
	const int n_iov = 2;
	
	iovec iov[ n_iov ] =
	{
		{ (void*) length, sizeof *length },
		{ (void*) domain, sizeof *domain },
	};
	
	return readv( fd, iov, n_iov );
}

static
ssize_t read_command_data( int fd, void* buffer, uint32_t length )
{
	return read( fd, buffer, length );
}

void read_into_queue( int fd )
{
	using iota::u16_from_big;
	using iota::u32_from_big;
	
	uint16_t domain;
	uint32_t length;
	
	ssize_t bytes = read_command_header( fd, &domain, &length );
	
	if ( bytes < 0 )
	{
		WARN( "bytes < 0" );
		
		_exit( 1 );
	}
	
	if ( bytes == 0 )
	{
		_exit( 0 );
	}
	
	length = iota::u32_from_big( length );
	
	switch ( domain )
	{
		case basic_domain:
			{
				basic_buffer buffer;
				
				if ( length != sizeof buffer )
				{
					WARN( "length != sizeof basic_buffer" );
					
					_exit( 1 );
				}
				
				bytes = read_command_data( fd, &buffer, length );
				
				switch ( buffer.code )
				{
					case 0xFFFF:
						// This command is special-- it clears the queue, aborting all pending sounds
						clear_queue();
						break;
				}
			}
			break;
		
		case sound_domain:
			{
				sound_buffer& buffer = add_to_queue();
				
				if ( length > sizeof buffer )
				{
					WARN( "length > sizeof sound_buffer" );
					
					_exit( 1 );
				}
				
				bytes = read_command_data( fd, &buffer, length );
				
				buffer.mode = u16_from_big( buffer.mode );
				
				switch ( buffer.mode )
				{
					case swMode:
						for ( Tone* tone = buffer.square_wave.rec.triplets; tone->count != 0 && tone->amplitude != 0 && tone->duration != 0; ++tone )
						{
							tone->count     = u16_from_big( tone->count     );
							tone->amplitude = u16_from_big( tone->amplitude );
							tone->duration  = u16_from_big( tone->duration  );
						}
						
						buffer.square_wave.current_tone = &buffer.square_wave.rec.triplets[ 0 ];
						buffer.square_wave.elapsed_tone_samples = 0;
						
						break;
					
					case ftMode:
						// TODO
						break;
					
					case ffMode:
						buffer.free_form.rec.count = u32_from_big( buffer.free_form.rec.count );
						buffer.free_form.size = length - ( sizeof buffer.free_form.rec.mode + sizeof buffer.free_form.rec.count );
						buffer.free_form.elapsed_samples = 0;
						
						break;
					
					default:
						WARN( "unsupported synthesizer mode" );
						break;
				}
			}
			break;
		
		default:
			WARN( "unsupported domain" );
			
			_exit( 1 );
	}
}
