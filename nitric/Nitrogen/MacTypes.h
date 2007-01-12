// MacTypes.h

#ifndef NITROGEN_MACTYPES_H
#define NITROGEN_MACTYPES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACTYPES__
#include FRAMEWORK_HEADER(CarbonCore,MacTypes.h)
#endif
#ifndef __SCRIPT__
#include FRAMEWORK_HEADER(CarbonCore,Script.h)
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif
#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
#endif
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_OVERLOADED_MATH_H
#include "Nucleus/Overloaded_math.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif
#ifndef NUCLEUS_MAKE_H
#include "Nucleus/Make.h"
#endif
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif
#ifndef NUCLEUS_FLATTENER_H
#include "Nucleus/Flattener.h"
#endif

#include <cstddef>
#include <string>
#include <cmath>


// Temporary measure until this code moves elsewhere
#if !TARGET_API_MAC_OSX

inline UInt32 CFSwapInt32BigToHost( UInt32 x )  { return x; }
inline UInt32 CFSwapInt32HostToBig( UInt32 x )  { return x; }

#endif


namespace Nitrogen
  {
   using ::UInt8;
   using ::SInt8;
   using ::UInt16;
   using ::SInt16;
   using ::UInt32;
   using ::SInt32;

   typedef long long                wide;
   typedef unsigned long long       UnsignedWide;


	namespace Detail
	{
		
		inline UInt32 Distance( const void *begin, const void *end )
		{
			return static_cast< UInt32 >(   static_cast< const char * >( end   )
										  - static_cast< const char * >( begin ) );
		}
		
	}
	
	
   // Nitrogen uses floating point types in preference to fixed-point types.
   template < class Floating, int fractionBits, class Integral >
   Floating FixedToFloatingPoint( Integral in )
     {
      return std::ldexp( static_cast<Floating>(in), -fractionBits );
     }

   template < class Integral, int fractionBits, class Floating >
   Integral FloatingToFixedPoint( Floating in )
     {
      return static_cast< Integral >( Nucleus::CStd::nearbyint( std::ldexp( in, fractionBits ) ) );
     }
   
   inline double FixedToDouble( ::Fixed in )                   { return FixedToFloatingPoint< double,  16 >( in ); }
   inline ::Fixed DoubleToFixed( double in )                   { return FloatingToFixedPoint< ::Fixed, 16 >( in ); }

   inline double UnsignedFixedToDouble( ::UnsignedFixed in )   { return FixedToFloatingPoint< double,          16 >( in ); }
   inline ::UnsignedFixed DoubleToUnsignedFixed( double in )   { return FloatingToFixedPoint< ::UnsignedFixed, 16 >( in ); }

   inline double FractToDouble( ::Fract in )                   { return FixedToFloatingPoint< double,  30 >( in ); }
   inline ::Fract DoubleToFract( double in )                   { return FloatingToFixedPoint< ::Fract, 30 >( in ); }

   inline double ShortFixedToDouble( ::ShortFixed in )         { return FixedToFloatingPoint< double,       8 >( in ); }
   inline ::ShortFixed DoubleToShortFixed( double in )         { return FloatingToFixedPoint< ::ShortFixed, 8 >( in ); }
   
   struct FixedFlattener
     {
      typedef double Put_Parameter;
      
      template < class Putter > void Put( Put_Parameter toPut, Putter put )
        {
         const Fixed fixed = DoubleToFixed( toPut );
         put( &fixed, &fixed + 1 );
        }
      
      typedef double Get_Result;
      
      template < class Getter > Get_Result Get( Getter get )
        {
         Fixed fixed;
         get( &fixed, &fixed + 1 );
         return FixedToDouble( fixed );
        }
      
      typedef Put_Parameter Parameter;
      typedef Get_Result    Result;
      
      static const bool hasStaticSize = true;
      
      typedef ::Fixed Buffer;
     };
   
   
   using ::Float32;
   using ::Float64;
   using ::Float80;
   using ::Float96;
   
   typedef ::std::size_t  Size;

   class OptionBitsTag {};
   typedef Nucleus::FlagType< OptionBitsTag, ::OptionBits, 0 > OptionBits;
   
   class ScriptCodeTag {};
   typedef Nucleus::SelectorType< ScriptCodeTag, ::ScriptCode, ::smSystemScript > ScriptCode;

   class LangCodeTag {};
   typedef Nucleus::SelectorType< LangCodeTag,   ::LangCode,   ::langUnspecified > LangCode;

   class RegionCodeTag {};
   typedef Nucleus::SelectorType< RegionCodeTag, ::RegionCode, ::verUS > RegionCode;
	
	class FourCharCode
	{
		private:
			::FourCharCode itsValue;
		
		public:
			FourCharCode()  {}
			
			FourCharCode( ::FourCharCode value ) : itsValue( value )  {}
			
			FourCharCode& operator=( ::FourCharCode value )  { itsValue = value;  return *this; }
			
			::FourCharCode Get() const  { return itsValue; }
			
			operator ::FourCharCode() const  { return Get(); }
	};
	
   class OSTypeTag {};
   typedef Nucleus::SelectorType< OSTypeTag, ::OSType, ::kUnknownType > OSType;
   
   typedef Nucleus::Selector< class ResType_Tag, ::ResType >::Type ResType;

   typedef bool Boolean;
   
   typedef Nucleus::ConvertingPODFlattener< bool, ::Boolean > BooleanFlattener;

   class StyleTag {};
   typedef Nucleus::FlagType< StyleTag, ::Style > Style;
      //Style may need a conversion to short...

   using ::UnicodeScalarValue;
   using ::UTF32Char;
   using ::UniChar;
   using ::UTF16Char;
   using ::UTF8Char;

   typedef std::basic_string< UTF32Char > UTF32String;
   typedef std::basic_string< UTF16Char > UTF16String;
   typedef std::basic_string< UTF8Char > UTF8String;
   typedef std::basic_string< UniChar > UniString;
   
	template < class UTFChar >
	struct UnicodeFlattener
	{
		typedef const std::basic_string< UTFChar >& Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			const UTFChar* begin = &toPut[0];
			const std::size_t size = toPut.size() * sizeof (UTFChar);
			
			put( begin, begin + size );
		}
		
		typedef std::basic_string< UTFChar > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result;
			
			result.resize( size / sizeof (UTFChar) );
			
			UTFChar* begin = &result[0];
			
			get( begin, begin + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
   
   using ::Point;
   using ::Rect;
  }

namespace Nucleus
  {
   template <>
   struct Maker< Nitrogen::Point >
     {
      Point operator()( short v, short h ) const
        {
         Point result;
         result.v = v;
         result.h = h;
         return result;
        }
      
      Point operator()() const
        {
         return operator()( 0, 0 );
        }
     };

   template <>
   struct Maker< Nitrogen::Rect >
     {
      Rect operator()( short top, short left, short bottom, short right ) const
        {
         Rect result;
         result.top = top;
         result.left = left;
         result.bottom = bottom;
         result.right = right;
         return result;
        }
      
      Rect operator()() const
        {
         return operator()( 0, 0, 0, 0 );
        }
     };

	// Convert string to FourCharCode
	template < class Tag, ::FourCharCode defaultValue >
	struct Converter< SelectorType< Tag, ::FourCharCode, defaultValue >, std::string >: public std::unary_function< std::string, SelectorType< Tag, ::FourCharCode, defaultValue > >
	{
		typedef Nucleus::SelectorType< Tag, ::FourCharCode, defaultValue > Code;
		
		Code operator()( const std::string& input ) const
		{
			if ( input.size() != sizeof (::FourCharCode) )
			{
				throw ConversionFromStringFailed();
			}
			
			::FourCharCode result;
			
			std::copy( input.begin(), input.end(), reinterpret_cast< char* >( &result ) );
			
			if ( TARGET_RT_LITTLE_ENDIAN )
			{
				result = ::CFSwapInt32BigToHost( result );
			}
			
			return Code( result );
		}
	};
	
	// Convert string to FourCharCode
	template <>
	struct Converter< Nitrogen::FourCharCode, std::string >: public std::unary_function< std::string, FourCharCode >
	{
		Nitrogen::FourCharCode operator()( const std::string& input ) const
		{
			if ( input.size() != sizeof (::FourCharCode) )
			{
				throw ConversionFromStringFailed();
			}
			
			::FourCharCode result;
			
			std::copy( input.begin(), input.end(), reinterpret_cast< char* >( &result ) );
			
			if ( TARGET_RT_LITTLE_ENDIAN )
			{
				result = ::CFSwapInt32BigToHost( result );
			}
			
			return Nitrogen::FourCharCode( result );
		}
	};
	
	// Convert FourCharCode to string
	template < class Tag, ::FourCharCode defaultValue >
	struct Converter< std::string, SelectorType< Tag, ::FourCharCode, defaultValue > >: public std::unary_function< SelectorType< Tag, ::FourCharCode, defaultValue >, std::string >
	{
		typedef Nucleus::SelectorType< Tag, ::FourCharCode, defaultValue > Code;
		
		std::string operator()( Code input ) const
		{
			::FourCharCode code = input;
			
			if ( TARGET_RT_LITTLE_ENDIAN )
			{
				code = ::CFSwapInt32HostToBig( code );
			}
			
			return std::string( reinterpret_cast< const char* >( &code ), sizeof (::FourCharCode) );
		}
	};
	
	// Convert FourCharCode to string
	template <>
	struct Converter< std::string, FourCharCode >: public std::unary_function< FourCharCode, std::string >
	{
		std::string operator()( Nitrogen::FourCharCode input ) const
		{
			::FourCharCode code = input;
			
			if ( TARGET_RT_LITTLE_ENDIAN )
			{
				code = ::CFSwapInt32HostToBig( code );
			}
			
			return std::string( reinterpret_cast< const char* >( &code ), sizeof (::FourCharCode) );
		}
	};
	
  }

#endif
