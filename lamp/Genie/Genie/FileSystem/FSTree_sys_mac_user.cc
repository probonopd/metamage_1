/*	======================
 *	FSTree_sys_mac_user.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user.hh"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_home.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"


namespace Genie
{
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &sys_mac_user_name::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_user_Mappings[] =
	{
		{ "home", &New_FSTree_sys_mac_user_home },
		
		{ "name", &Name_Factory },
		
		{ NULL, NULL }
	};
	
}

