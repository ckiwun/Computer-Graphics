//
// slBrush.h
//
// The header file for Point Brush. 
//

#ifndef SLBRUSH_H
#define SLBRUSH_H

#include "impBrush.h"

class ScatteredLineBrush : public ImpBrush
{
public:
	ScatteredLineBrush( ImpressionistDoc* pDoc = NULL, const char* name = NULL );

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	char* BrushName( void );
};

#endif
