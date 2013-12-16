/*	===============
 *	PairedSocket.cc
 *	===============
 */

#include "Genie/IO/PairedSocket.hh"

// STREAMS
#ifdef __RELIX__
#include <stropts.h>
#endif

// POSIX
#include <fcntl.h>
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"
#include "vfs/filehandle/primitives/conveying.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/get_fd_handle.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class PairedSocket : public StreamHandle
	{
		private:
			boost::intrusive_ptr< vfs::stream >  itsInput;
			boost::intrusive_ptr< vfs::stream >  itsOutput;
		
		public:
			PairedSocket( boost::intrusive_ptr< vfs::stream >  input,
			              boost::intrusive_ptr< vfs::stream >  output,
			              bool                                 nonblocking );
			
			~PairedSocket();
			
			unsigned int SysPoll()
			{
				return   kPollRead  * itsInput->is_readable()
				       | kPollWrite * itsOutput->is_writable();
			}
			
			ssize_t SysRead( char* data, std::size_t byteCount )
			{
				return itsInput->read( data, byteCount, IsNonblocking(), &try_again );
			}
			
			ssize_t SysWrite( const char* data, std::size_t byteCount )
			{
				return itsOutput->write( data, byteCount, IsNonblocking(), &try_again, &broken_pipe );
			}
			
			void IOCtl( unsigned long request, int* argp );
			
			void ShutdownReading();
			void ShutdownWriting();
			
			bool conveying() const  { return !itsInput->empty(); }
	};
	
	
	static void pairedsocket_shutdown( vfs::filehandle* sock, int how )
	{
		if ( how != SHUT_WR )
		{
			static_cast< PairedSocket& >( *sock ).ShutdownReading();
		}
		
		if ( how != SHUT_RD )
		{
			static_cast< PairedSocket& >( *sock ).ShutdownWriting();
		}
	}
	
	static int pairedsocket_conveying( vfs::filehandle* sock )
	{
		return static_cast< PairedSocket& >( *sock ).conveying();
	}
	
	static const vfs::socket_method_set pairedsocket_socket_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&pairedsocket_shutdown,
		NULL,
		NULL,
		&pairedsocket_conveying,
	};
	
	static const vfs::filehandle_method_set pairedsocket_methods =
	{
		NULL,
		&pairedsocket_socket_methods,
	};
	
	
	PairedSocket::PairedSocket( boost::intrusive_ptr< vfs::stream >  input,
			                    boost::intrusive_ptr< vfs::stream >  output,
			                    bool                                 nonblocking )
	:
		StreamHandle( nonblocking ? O_RDWR | O_NONBLOCK
		                          : O_RDWR,
		              &pairedsocket_methods ),
		itsInput ( input  ),
		itsOutput( output )
	{
	}
	
	PairedSocket::~PairedSocket()
	{
		ShutdownReading();
		ShutdownWriting();
	}
	
	static vfs::filehandle& check_fh( vfs::filehandle& fh )
	{
		if ( conveying( fh ) )
		{
			// To prevent cycles, don't send a socket with fds in flight
			
			p7::throw_errno( ELOOP );
		}
		
		return fh;
	}
	
	void PairedSocket::IOCtl( unsigned long request, int* argp )
	{
		switch ( request )
		{
		#ifdef __RELIX__
			
			case I_SENDFD:
				// FIXME:  Implement garbage collection to allow cycles
				
				itsOutput->send_fd( check_fh( relix::get_fd_handle( (int) argp ) ) );
				
				break;
			
			case I_RECVFD:
				if ( argp != NULL )
				{
					strrecvfd* arg = (strrecvfd*) argp;
					
					int fd = relix::first_free_fd();
					
					relix::assign_fd( fd, *itsInput->recv_fd( IsNonblocking(), &try_again ) );
					
					arg->fd = fd;
					
					arg->uid = 0;
					arg->gid = 0;
					
					int* fill = (int*) arg->fill;
					
					fill[ 0 ] = 0;
					fill[ 1 ] = 0;
				}
				
				break;
			
		#endif
			
			default:
				p7::throw_errno( EINVAL );
				
				break;
		}
	}
	
	void PairedSocket::ShutdownReading()
	{
		itsInput->close_egress();
	}
	
	void PairedSocket::ShutdownWriting()
	{
		itsOutput->close_ingress();
	}
	
	
	IOPtr
	//
	NewPairedSocket( const boost::intrusive_ptr< vfs::stream >&  input,
	                 const boost::intrusive_ptr< vfs::stream >&  output,
	                 bool                                        nonblocking )
	{
		return new PairedSocket( input, output, nonblocking );
	}
	
}

