/*	====================
 *	FSTree_crm_serial.cc
 *	====================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_crm_serial.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"


namespace Genie
{
	
	static N::CRMRecPtr GetCRMRecPtrFromID( N::CRMDeviceID id )
	{
		N::CRMResource_Container crmResources = N::CRMResources( N::crmSerialDevice );
		
		typedef N::CRMResource_Container::const_iterator Iter;
		
		for ( Iter it = crmResources.begin();  it != crmResources.end();  ++it )
		{
			N::CRMRecPtr rec = *it;
			
			if ( rec->crmDeviceID == id )
			{
				return rec;
			}
		}
		
		return NULL;
	}
	
	std::string CRMDeviceID_KeyName_Traits::NameFromKey( const Key& key )
	{
		return NN::Convert< std::string >( key );
	}
	
	CRMDeviceID_KeyName_Traits::Key CRMDeviceID_KeyName_Traits::KeyFromName( const std::string& name )
	{
		return Key( std::atoi( name.c_str() ) );
	}
	
	
	bool sys_mac_crm_serial_Details::KeyIsValid( const Key& key )
	{
		// We can't just call N::CRMSearch< N::crmSerialDevice >( key ),
		// because that returns the *next* record.  So, linear search it is.
		
		return GetCRMRecPtrFromID( key ) != NULL;
	}
	
	FSTreePtr sys_mac_crm_serial_Details::Parent() const
	{
		return GetSingleton< FSTree_sys_mac_crm >();
	}
	
	
	class sys_mac_crm_serial_N_name_Query
	{
		private:
			typedef N::CRMDeviceID            Key;
			typedef StringHandle              CRMSerialRecord::*Selector;
			
			Key itsKey;
			
			Selector itsSelector;
		
		public:
			sys_mac_crm_serial_N_name_Query( const Key&  key,
			                                 Selector    selector ) : itsKey     ( key      ),
			                                                          itsSelector( selector )
			{
			}
			
			std::string operator()() const
			{
				N::CRMRecPtr crmRec = GetCRMRecPtrFromID( itsKey );
				
				N::CRMSerialPtr serialPtr = NN::Convert< N::CRMSerialPtr >( crmRec );
				
				StringHandle h = serialPtr->*itsSelector;
				
				std::string name = NN::Convert< std::string >( N::Str255( *h ) );
				
				std::string output = name + "\n";
				
				return output;
			}
	};
	
	class sys_mac_crm_serial_N_icon_Query
	{
		private:
			typedef N::CRMDeviceID Key;
			
			Key itsKey;
		
		public:
			sys_mac_crm_serial_N_icon_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string operator()() const
			{
				N::CRMRecPtr crmRec = GetCRMRecPtrFromID( itsKey );
				
				N::CRMSerialPtr serialPtr = NN::Convert< N::CRMSerialPtr >( crmRec );
				
				CRMIconHandle iconHandle = serialPtr->deviceIcon;
				
				CRMIconRecord icon = **iconHandle;
				
				// 32-bit * 32-bit = 1024 bits = 128 bytes, x2 = 256 bytes
				const std::size_t maskedIconSize = sizeof icon.oldIcon + sizeof icon.oldMask;
				
				std::string iconData( (const char*) &icon.oldIcon, maskedIconSize );
				
				return iconData;
			}
	};
	
	
	FSTree_sys_mac_crm_serial_N::FSTree_sys_mac_crm_serial_N( const Key& key ) : itsKey( key )
	{
	}
	
	static FSTreePtr String_Factory( const FSTreePtr&                 parent,
	                                 const std::string&               name,
	                                 CRMDeviceID_KeyName_Traits::Key  key,
	                                 StringHandle CRMSerialRecord::*  member )
	{
		typedef sys_mac_crm_serial_N_name_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key, member ) ) );
	}
	
	static FSTreePtr Name_Factory( const FSTreePtr&                 parent,
	                               const std::string&               name,
	                               CRMDeviceID_KeyName_Traits::Key  key )
	{
		return String_Factory( parent, name, key, &CRMSerialRecord::name );
	}
	
	static FSTreePtr Input_Factory( const FSTreePtr&                 parent,
	                                const std::string&               name,
	                                CRMDeviceID_KeyName_Traits::Key  key )
	{
		return String_Factory( parent, name, key, &CRMSerialRecord::inputDriverName );
	}
	
	static FSTreePtr Output_Factory( const FSTreePtr&                 parent,
	                                 const std::string&               name,
	                                 CRMDeviceID_KeyName_Traits::Key  key )
	{
		return String_Factory( parent, name, key, &CRMSerialRecord::outputDriverName );
	}
	
	static FSTreePtr Icon_Factory( const FSTreePtr&                 parent,
	                               const std::string&               name,
	                               CRMDeviceID_KeyName_Traits::Key  key )
	{
		typedef sys_mac_crm_serial_N_icon_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	void FSTree_sys_mac_crm_serial_N::Init()
	{
		FSTreePtr shared_this( shared_from_this() );
		
		Map( Name_Factory  ( shared_this, "name",   itsKey ) );
		Map( Input_Factory ( shared_this, "input",  itsKey ) );
		Map( Output_Factory( shared_this, "output", itsKey ) );
		Map( Icon_Factory  ( shared_this, "icon",   itsKey ) );
	}
	
}

#endif

