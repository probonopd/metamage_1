/*	=====================
 *	FSTree_dev_gestalt.cc
 *	=====================
 */

#include "Genie/FS/FSTree_dev_gestalt.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class GestaltDeviceHandle : public vfs::filehandle
	{
		public:
			GestaltDeviceHandle( int flags ) : vfs::filehandle( flags )
			{
			}
			
			vfs::node_ptr GetFile()  { return vfs::resolve_absolute_path( STR_LEN( "/dev/gestalt" ) ); }
			
			void IOCtl( unsigned long request, int* argp );
	};
	
	
	void GestaltDeviceHandle::IOCtl( unsigned long request, int* argp )
	{
		long value;
		
		const OSErr err = ::Gestalt( request, &value );
		
		switch ( err )
		{
			case noErr:
				break;
			
			default:  // shouldn't happen
			case gestaltUndefSelectorErr:  p7::throw_errno( EINVAL );
			case gestaltUnknownErr      :  p7::throw_errno( ENXIO  );
		}
		
		if ( argp != NULL )
		{
			*argp = value;
		}
	}
	
	vfs::filehandle_ptr dev_gestalt::open( const vfs::node* that, int flags, mode_t mode )
	{
		return new GestaltDeviceHandle( flags );
	}
	
}

