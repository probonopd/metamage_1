/*
	Vertice/PortView.cc
	-------------------
*/

#include "Vertice/PortView.hh"

// Standard C++
#include <vector>

// nucleus
#include "nucleus/saved.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Portage
#include "Portage/DepthBuffer.hh"

// Vertice
#include "Vertice/Render.hh"


namespace Vertice
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	PortView::PortView( const Rect& bounds ) : itsBounds         ( bounds      ),
	                                           itsPort           ( itsScene    ),
	                                           itsSelectedContext(             ),
	                                           itsAnaglyphMode   ( kNoAnaglyph )
	{
		SetBounds( bounds );
	}
	
	
	static
	void paint_into_GWorld( const std::vector< MeshModel >&  models,
	                        GWorldPtr                        gworld )
	{
		PixMapHandle pix = N::GetGWorldPixMap( gworld );
		
		const Rect  bounds = ( *pix )->bounds;
		::Ptr       base   = ( *pix )->baseAddr;
		unsigned    stride = ( *pix )->rowBytes & 0x3fff;
		
		const unsigned width  = bounds.right - bounds.left;
		const unsigned height = bounds.bottom - bounds.top;
		
		memset( base, '\0', height * stride );
		
		paint_onto_surface( models, base, width, height, stride );
	}
	
	static
	void blit_to_thePort( CGrafPtr src )
	{
		CGrafPtr thePort = N::GetQDGlobalsThePort();
		
		const Rect bounds = N::GetPortBounds( thePort );
		
		PixMapHandle pix = N::GetGWorldPixMap( thePort );
		n::saved< N::Pixels_State > savedPixelsState( pix );
		N::LockPixels( pix );
		
		N::CopyBits( N::GetPortBitMapForCopyBits( src ),
		             N::GetPortBitMapForCopyBits( thePort ),
		             N::GetPortBounds( src ),
		             bounds,
		             N::srcCopy );
		
		if ( TARGET_API_MAC_CARBON )
		{
			::QDFlushPortBuffer( ::GetQDGlobalsThePort(), N::RectRgn( bounds ) );
		}
	}
	
	void PortView::Draw( const Rect& bounds, bool erasing )
	{
		if ( itsAnaglyphMode )
		{
			DrawAnaglyphic();
		}
		else
		{
			itsPort.MakeFrame( itsFrame );
			
			paint_into_GWorld( itsFrame.Models(), itsGWorld );
		}
		
		blit_to_thePort( itsGWorld );
	}
	
	void PortView::DrawAnaglyphic()
	{
		std::size_t contextIndex = itsScene.Cameras().front().ContextIndex();
		
		Moveable& target = itsPort.itsScene.GetContext( contextIndex );
		
		double eyeRadius = 0.05;  // distance from eye to bridge of nose
		
		nucleus::owned< GWorldPtr > altGWorld = N::NewGWorld( 32, itsBounds );
		
		N::LockPixels( N::GetGWorldPixMap( altGWorld ) );
		
		
		target.ContextTranslate( -eyeRadius, 0, 0 );
		
		itsPort.MakeFrame( itsFrame );
		
		paint_into_GWorld( itsFrame.Models(), altGWorld );
		
		
		target.ContextTranslate( 2 * eyeRadius, 0, 0 );
		
		itsPort.MakeFrame( itsFrame );
		
		paint_into_GWorld( itsFrame.Models(), itsGWorld );
		
		
		target.ContextTranslate( -eyeRadius, 0, 0 );
		
		PixMapHandle pixL = N::GetGWorldPixMap( altGWorld );
		PixMapHandle pixR = N::GetGWorldPixMap( itsGWorld );
		
		::Ptr baseL = pixL[0]->baseAddr;
		::Ptr baseR = pixR[0]->baseAddr;
		
		unsigned width  = itsBounds.right - itsBounds.left;
		unsigned height = itsBounds.bottom - itsBounds.top;
		
		unsigned stride = pixL[0]->rowBytes & 0x3fff;
		
		merge_anaglyph( itsAnaglyphMode, baseL, baseR, height, width, stride );
	}
	
	
	bool PortView::DispatchCursor( const EventRecord& event )
	{
		N::SetCursor( N::GetCursor( N::crossCursor ) );
		
		return true;
	}
	
	void PortView::DrawBetter() const
	{
		PixMapHandle pix = ::GetPortPixMap( itsGWorld );
		
		const Rect& portRect = N::GetPortBounds( itsGWorld );
		
		const Rect& pixBounds = ( *pix )->bounds;
		::Ptr       baseAddr  = ( *pix )->baseAddr;
		unsigned    rowBytes  = ( *pix )->rowBytes & 0x3fff;
		
		memset( baseAddr, '\0', rowBytes * (pixBounds.bottom - pixBounds.top) );
		
		const short width  = portRect.right - portRect.left;
		const short height = portRect.bottom - portRect.top;
		
		trace_onto_surface( itsFrame.Models(), baseAddr, width, height, rowBytes );
		
		blit_to_thePort( itsGWorld );
	}
	
	bool PortView::MouseDown( const EventRecord& event )
	{
		Point macPt = N::GlobalToLocal( event.where );
		
		itsPort.MakeFrame( itsFrame );
		
		const int width  = itsBounds.right - itsBounds.left;
		const int height = itsBounds.bottom - itsBounds.top;
		
		const double x = (macPt.h + 0.5 - width  / 2.0) / (width /  2.0);
		const double y = (macPt.v + 0.5 - height / 2.0) / (width / -2.0);
		
		MeshModel* model = hit_test( itsFrame, x, y );
		
		if ( model != NULL )
		{
			model->Select();
		}
		
		paint_into_GWorld( itsFrame.Models(), itsGWorld );
		
		blit_to_thePort( itsGWorld );
		
		return true;
	}
	
	bool PortView::KeyDown( const EventRecord& event )
	{
		char c = event.message & charCodeMask;
		
		if ( 0 )
		{
			//short code = (inEvent.message & keyCodeMask) >> 8;
			//short code = inEvent.KeyCode();
		}
		else
		{
			return KeyDown( c );
		}
	}
	
	bool PortView::KeyDown( char c )
	{
		if ( c == '~' )
		{
			DrawBetter();
			
			return true;
		}
		
		short cmd = cmdNone;
		
		bool shooter = true;
		if ( shooter )
		{
			switch ( c )
			{
				case 0x1e: // up arrow
					c = '0';
				break;
				case 0x1f: // down arrow
					c = '.';
				break;
			}
		}
		else
		{
			switch ( c )
			{
				case 0x1e: // up arrow
					c = '8';
					break;
				case 0x1f: // down arrow
					c = '5';
					break;
			}
		}
		switch ( c )
		{
			case 'a':
				itsAnaglyphMode = kNoAnaglyph;
				break;
			
			case 's':
				itsAnaglyphMode = kTrueAnaglyph;
				break;
			
			case 'd':
				itsAnaglyphMode = kGrayAnaglyph;
				break;
			
			case 'f':
				itsAnaglyphMode = kColorAnaglyph;
				break;
			
			case 'g':
				itsAnaglyphMode = kHalfColorAnaglyph;
				break;
			
			case 'h':
				itsAnaglyphMode = kOptimizedAnaglyph;
				break;
			
			case '7':
				cmd = cmdMoveLeft;
				break;
			case '9':
				cmd = cmdMoveRight;
				break;
			case '4':
			case 0x1c: // left arrow
				cmd = cmdYawLeft;
				break;
			case '6':
			case 0x1d: // right arrow
				cmd = cmdYawRight;
				break;
			case '1':
				cmd = cmdRollLeft;
				break;
			case '3':
				cmd = cmdRollRight;
				break;
			case '8':
				cmd = cmdPitchDown;
				break;
			case '5':
				cmd = cmdPitchUp;
				break;
			case '0':
				cmd = cmdMoveForward;
				break;
			case '.':
				cmd = cmdMoveBackward;
				break;
			case '-':
				cmd = cmdMoveUp;
				break;
			case '+':
				cmd = cmdMoveDown;
				break;
			case '*':
				cmd = cmdExpand;
				break;
			case '/':
				cmd = cmdContract;
				break;
			case '2':
				cmd = cmdLevelPitch;
				break;
			case '=':
				cmd = cmdLevelRoll;
				break;
			case '_':
			case 0x03:
				cmd = cmdGroundHeight;
				break;
			case ' ':
				cmd = cmdPlayPause;
				break;
			case 'p':
				//cmd = cmdProjectionMode;
				break;
			case '[':
				cmd = cmdPrevCamera;
				break;
			case ']':
				cmd = cmdNextCamera;
				break;
			default:
				return false;
				break;
		}
		
		if ( itsSelectedContext == 0 )
		{
			itsSelectedContext = itsScene.Cameras().front().ContextIndex();
		}
		
		itsPort.SendCameraCommand( itsSelectedContext, cmd );
		
		Draw( Rect(), true );
		
		return true;
	}
	
	void PortView::SetBounds( const Rect& bounds )
	{
		itsBounds = bounds;
		
		itsGWorld = N::NewGWorld( 32, itsBounds );
		
		N::LockPixels( N::GetGWorldPixMap( itsGWorld ) );
	}
	
}
