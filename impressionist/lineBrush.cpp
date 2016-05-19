//
// lineBrush.cpp
//
// The implementation of line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include <math.h>

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "lineBrush.h"

#define PI 3.14159265

extern float frand();

LineBrush::LineBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void LineBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize( (float)size );

	BrushMove( source, target );
}

void LineBrush::BrushMove( const Point source, const Point target )
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
	int grad_x[9] = {-1,0,1,-2,0,2,-1,0,1};
	int grad_y[9] = {-1,-2,-1,0,0,0,1,2,1};
	if(dlg->m_DirectionButton->value())
		angle = pDoc->getDirAngle();
	else if(dlg->m_GradientButton->value()){
		int x = pDoc->drag_end.x;
		int y = pDoc->drag_end.y;
		int idx,idy;
		float d_x,d_y,gray_temp;
		d_x=d_y=0;
		for(int i=0;i<3;i++)
		for(int j=0;j<3;j++){
			idx=x+j-1;
			idy=y+i-1;
			pDoc->round_index(&idx,pDoc->m_nPaintWidth);
			pDoc->round_index(&idy,pDoc->m_nPaintHeight);
			gray_temp =	0.299*pDoc->m_ucBitmap[3*(idy*pDoc->m_nPaintWidth+idx)] +
					0.587*pDoc->m_ucBitmap[3*(idy*pDoc->m_nPaintWidth+idx)+1] +
					0.114*pDoc->m_ucBitmap[3*(idy*pDoc->m_nPaintWidth+idx)+2] ;
			d_x+=grad_x[i*3+j]*gray_temp;
			d_y+=grad_y[i*3+j]*gray_temp;
			angle = atan(d_y/d_x)*180/PI;
			if(d_x>=0&&d_y>=0||d_x<0&&d_y<0)
				angle = 90-angle;
			else
				angle = 270-angle;
		}
	}
	int Ax,Ay,Bx,By,Cx,Cy,Dx,Dy;

	float cos_angle,sin_angle;
	float rad_angle = (angle*PI)/180;

	cos_angle = cos(-rad_angle);
	sin_angle = sin(-rad_angle);

	Ax = target.x + ( (-linesize)*cos_angle - (-size)*sin_angle );
	Bx = target.x + ( ( linesize)*cos_angle - (-size)*sin_angle );
	Cx = target.x + ( ( linesize)*cos_angle - ( size)*sin_angle );
	Dx = target.x + ( (-linesize)*cos_angle - ( size)*sin_angle );
	Ay = target.y + ( (-linesize)*sin_angle + (-size)*cos_angle );
	By = target.y + ( ( linesize)*sin_angle + (-size)*cos_angle );
	Cy = target.y + ( ( linesize)*sin_angle + ( size)*cos_angle );
	Dy = target.y + ( (-linesize)*sin_angle + ( size)*cos_angle );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glBegin( GL_POLYGON );
		SetColor( source, alpha );
		glVertex2i( Ax, Ay ) ;
		glVertex2i( Bx, By ) ;
		glVertex2i( Cx, Cy ) ;
		glVertex2i( Dx, Dy ) ;
	glEnd();
}

void LineBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

