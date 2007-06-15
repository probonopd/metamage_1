// Nitrogen/Components.cp
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
#endif

namespace Nitrogen
{
	
	ComponentManagerErrorsRegistrationDependency::ComponentManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterComponentManagerErrors();
	
	
	class ComponentManagerErrorsRegistration
	{
		public:
			ComponentManagerErrorsRegistration()  { RegisterComponentManagerErrors(); }
	};
	
	static ComponentManagerErrorsRegistration theRegistration;
	
	
	Nucleus::Owned< ComponentInstance >
	OpenDefaultComponent( ComponentType     componentType,
	                      ComponentSubType  componentSubType )
	{
		ComponentInstance component = ::OpenDefaultComponent( componentType,
		                                                      componentSubType );
		if ( component == NULL )
		{
			throw OpenDefaultComponent_Failed();
		}
		
		return Nucleus::Owned< ComponentInstance >::Seize( component );
	}
	
	void RegisterComponentManagerErrors()
	{
		RegisterOSStatus< resFNotFound         >();
		RegisterOSStatus< invalidComponentID   >();
		RegisterOSStatus< validInstancesExist  >();
		RegisterOSStatus< componentNotCaptured >();
		RegisterOSStatus< badComponentInstance >();
		RegisterOSStatus< badComponentSelector >();
	}
	
}

