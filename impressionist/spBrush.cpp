//
// spBrush.cpp
//
// The implementation of scatter point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include <math.h>

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "spBrush.h"
#include <iostream>

#define PI 3.14159265
#define DENSITY 1

extern float frand();

ScatteredPointBrush::ScatteredPointBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredPointBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

//	int size = pDoc->getSize();



//	glPointSize( (float)size );
	glPointSize( 1 );

	BrushMove( source, target );
}

void ScatteredPointBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "PointBrush::BrushMove  document is NULL\n" );
		return;
	}

	int size = pDoc->getSize();
	float alpha = pDoc->getAlpha();
	int angle = pDoc->getAngle();
	if(dlg->getDirMode()==1)
		angle = pDoc->getDirAngle();

	float cos_angle,sin_angle;
	float rad_angle = (angle*PI)/180;
	
	cos_angle = cos(-rad_angle);
	sin_angle = sin(-rad_angle);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	float orig_x,orig_y,x,y;
	SetColor( source, alpha );
	glBegin( GL_POINTS );
		for(int i = 0;i<0.05*size*size;i++)
		while(true){
			orig_x=-(size/2)+size*frand();
			orig_y=-(size/2)+size*frand();
			x = orig_x*cos_angle-orig_y*sin_angle;
			y = orig_x*sin_angle+orig_y*cos_angle;
			glVertex2d( target.x+x, target.y+y );
			break;
		}
	glEnd();
}

void ScatteredPointBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

