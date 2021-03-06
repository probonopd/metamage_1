/*
	Icons.hh
	--------
*/

#ifndef ICONS_HH
#define ICONS_HH

struct Rect;

pascal char** GetIcon_patch( short iconID );

pascal void PlotIcon_patch( const Rect* rect, char** icon );

void IconDispatch_patch( short method : __D0 );

#endif
