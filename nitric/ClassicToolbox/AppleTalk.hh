// AppleTalk.hh

#ifndef CLASSICTOOLBOX_APPLETALK_HH
#define CLASSICTOOLBOX_APPLETALK_HH

// Mac OS
#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif

// nucleus
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif

// Nitrogen
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( AppleTalk );
	
}

namespace nucleus
{
	
	template <>  struct maker< EntityName >
	{
		EntityName operator()( ConstStr32Param  object,
		                       ConstStr32Param  type,
		                       ConstStr32Param  zone = "\p*" ) const
		{
			EntityName entityName;
			
			Nitrogen::CopyToPascalString( object, entityName.objStr,  32 );
			Nitrogen::CopyToPascalString( type,   entityName.typeStr, 32 );
			Nitrogen::CopyToPascalString( zone,   entityName.zoneStr, 32 );
			
			return entityName;
		}
	};
	
}

namespace Nitrogen
{
	
	static EntityName NBPSetEntity( ConstStr32Param  object,
	                                ConstStr32Param  type,
	                                ConstStr32Param  zone = "\p*" )
	{
		EntityName entityName;
		::NBPSetEntity( reinterpret_cast< char* >( &entityName ), object, type, zone );
		
		return entityName;
	}
	
}

#endif
