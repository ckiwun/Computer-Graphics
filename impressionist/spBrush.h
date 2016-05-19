//
// spBrush.h
//
// The header file for Point Brush. 
//

#ifndef SPBRUSH_H
#define SPBRUSH_H

#include "impBrush.h"

class ScatteredPointBrush : public ImpBrush
{
public:
	ScatteredPointBrush( ImpressionistDoc* pDoc = NULL, const char* name = NULL );

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	char* BrushName( void );
};

#endif
