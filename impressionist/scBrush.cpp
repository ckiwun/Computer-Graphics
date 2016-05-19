//
// scBrush.cpp
//
// The implementation of scatter circle Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include <math.h>

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "scBrush.h"

#define PI 3.14159265

extern float frand();

ScatteredCircleBrush::ScatteredCircleBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredCircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize( (float)size );

	BrushMove( source, target );
}

void ScatteredCircleBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "PointBrush::BrushMove  document is NULL\n" );
		return;
	}

	int size = pDoc->getSize();
	float alpha = pDoc->getAlpha();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	double c_x,c_y,radius;
	c_x=target.x-(size/2)+size*frand();
	c_y=target.y-(size/2)+size*frand();
	radius = 4+size/15;
	SetColor( source, alpha );
	double angle,x,y;
	for(int i = 0; i <= 360; i++){
		glBegin( GL_LINES );
			angle = 2*PI*i/360;
			x=c_x+radius*cos(angle);
			y=c_y+radius*sin(angle);
			glVertex2d(c_x,c_y);
			glVertex2d(x,y);
		glEnd();
	}
}

void ScatteredCircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

