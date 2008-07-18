/*	========================
 *	FSTree_sys_mac_window.cc
 *	========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_window.hh"

// Standard C++
#include <algorithm>


namespace Genie
{
	
	bool sys_mac_window_Details::KeyIsValid( const Key& key )
	{
		const Sequence& sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	FSTreePtr sys_mac_window_Details::GetChildNode( const FSTreePtr& parent, const Key& key )
	{
		return MakeFSTree( new FSTree_sys_mac_window_REF( parent, key ) );
	}
	
}

