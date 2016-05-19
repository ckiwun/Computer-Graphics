//
// slBrush.cpp
//
// The implementation of scatter line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include <math.h>

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "slBrush.h"

#define PI 3.14159265

extern float frand();

ScatteredLineBrush::ScatteredLineBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredLineBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize( (float)size );

	BrushMove( source, target );
}

void ScatteredLineBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "PointBrush::BrushMove  document is NULL\n" );
		return;
	}
	int size = pDoc->getSize();
	float alpha = pDoc->getAlpha();
	int linesize = pDoc->getLinesize();
	int angle = pDoc->getAngle();
	if(dlg->getDirMode()==1)
		angle = pDoc->getDirAngle();
	double Ax,Ay,Bx,By,Cx,Cy,Dx,Dy;

	float cos_angle,sin_angle;
	float rad_angle = (angle*PI)/180;

	cos_angle = cos(-rad_angle);
	sin_angle = sin(-rad_angle);

	double x1,y1,x2,y2;
	do{
		x1 = -(size/2)+size*frand();
		y1 = -(size/2)+size*frand();
		x2 = -(size/2)+size*frand();
		y2 = -(size/2)+size*frand();
	} while(!( (x1>=0&y1>=0&x2<0&y2<0) || 
		   (x1>=0&y1<0&x2<0&y2>=0) ) );
	
	double ax1,ay1,ax2,ay2;	

	ax1 = target.x + x1*cos_angle - y1*sin_angle;
	ay1 = target.y + x1*sin_angle + y1*cos_angle;
	ax2 = target.x + x2*cos_angle - y2*sin_angle;
	ay2 = target.y + x2*sin_angle + y2*cos_angle;
	
	double slope = (ay2-ay1)/(ax2-ax1);

	if(slope < 1 && slope > -1){
		Ax = ax1;
		Ay = ay1-linesize;
		Bx = ax1;
		By = ay1+linesize;
		Cx = ax2;
		Cy = ay2+linesize;
		Dx = ax2;
		Dy = ay2-linesize;
	}
	else {
		Ax = ax1-linesize;
		Ay = ay1;
		Bx = ax1+linesize;
		By = ay1;
		Cx = ax2+linesize;
		Cy = ay2;
		Dx = ax2-linesize;
		Dy = ay2;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glBegin( GL_POLYGON );
		SetColor( source, alpha );
		glVertex2d( Ax, Ay ) ;
		glVertex2d( Bx, By ) ;
		glVertex2d( Cx, Cy ) ;
		glVertex2d( Dx, Dy ) ;
	glEnd();
}

void ScatteredLineBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

