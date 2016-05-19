//
// circleBrush.cpp
//
// The implementation of circle Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include <math.h>

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "circleBrush.h"

#define PI 3.14159265

extern float frand();

CircleBrush::CircleBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void CircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize( (float)size );

	BrushMove( source, target );
}

void CircleBrush::BrushMove( const Point source, const Point target )
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
	//glEnable(GL_LINE_SMOOTH);//enable anti-alias
	size /= 2;
	SetColor( source, alpha );
	double angle,x,y;
	for(int i = 0; i <= 360; i++){
		glBegin( GL_LINES );
			angle = 2*PI*i/360;
			x=target.x+size*cos(angle);
			y=target.y+size*sin(angle);
			glVertex2d(target.x,target.y);
			glVertex2d(x,y);
		glEnd();
	}
}

void CircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

