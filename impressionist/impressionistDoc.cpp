// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//


#include <FL/fl_ask.H>

#include <algorithm>
#include <math.h>
#include <iostream>

#include "impressionistDoc.h"
#include "impressionistUI.h"

#include "impBrush.h"

// Include individual brush headers here.
#include "pointBrush.h"
#include "lineBrush.h"
#include "spBrush.h"
#include "slBrush.h"
#include "scBrush.h"
#include "circleBrush.h"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }
#define PI 3.14159265

ImpressionistDoc::ImpressionistDoc() 
{
	// Set NULL image name as init. 
	m_imageName[0]	='\0';	

	m_nWidth		= -1;
	m_ucBitmap		= NULL;
	m_ucPainting	= NULL;
	m_ucPreviewBackup = NULL;


	// create one instance of each brush
	ImpBrush::c_nBrushCount	= NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes	= new ImpBrush* [ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS]	= new PointBrush( this, "Points" );

	// Note: You should implement these 5 brushes.  They are set the same (PointBrush) for now
	ImpBrush::c_pBrushes[BRUSH_LINES]				
		= new LineBrush( this, "Lines" );
	ImpBrush::c_pBrushes[BRUSH_CIRCLES]				
		= new CircleBrush( this, "Circles" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]	
		= new ScatteredPointBrush( this, "Scattered Points" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]		
		= new ScatteredLineBrush( this, "Scattered Lines" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]	
		= new ScatteredCircleBrush( this, "Scattered Circles" );

	// make one of the brushes current
	m_pCurrentBrush	= ImpBrush::c_pBrushes[0];
}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName() 
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush	= ImpBrush::c_pBrushes[type];
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}

float ImpressionistDoc::getAlpha()
{
	return m_pUI->getAlpha();
}

int ImpressionistDoc::getLinesize()
{
	return m_pUI->getLinesize();
}

int ImpressionistDoc::getAngle()
{
	return m_pUI->getAngle();
}

int ImpressionistDoc::getDirAngle()
{
	return m_nDirAngle;
}

void ImpressionistDoc::setDirAngle()
{
	float diff_x,diff_y,slope; 
	diff_y = (float)(drag_end.y-drag_start.y);
	diff_x = (float)(drag_end.x-drag_start.x);
	slope = diff_y/diff_x;
	m_nDirAngle = atan(slope)*180/PI;
	if(diff_x>=0&&diff_y>=0)
		m_nDirAngle = 90-m_nDirAngle;
	else if(diff_x<0&&diff_y>=0)
		m_nDirAngle = 270-m_nDirAngle;
	else if(diff_x<0&&diff_y<0)
		m_nDirAngle = 90-m_nDirAngle;
	else
		m_nDirAngle = 270-m_nDirAngle;
	//printf("the arc tan value of %f is %f degrees\n",slope,(float)m_nAngle);
}

void ImpressionistDoc::setDragAngle()
{
	float diff_x,diff_y,slope; 
	diff_y = (float)(drag_end.y-drag_start.y);
	diff_x = (float)(drag_end.x-drag_start.x);
	slope = diff_y/diff_x;
	m_nAngle = atan(slope)*180/PI;
	if(diff_x>=0&&diff_y>=0)
		m_nAngle = 90-m_nAngle;
	else if(diff_x<0&&diff_y>=0)
		m_nAngle = 270-m_nAngle;
	else if(diff_x<0&&diff_y<0)
		m_nAngle = 90-m_nAngle;
	else
		m_nAngle = 270-m_nAngle;
	m_pUI->setAngle(m_nAngle);
	//printf("the arc tan value of %f is %f degrees\n",slope,(float)m_nAngle);
}

void ImpressionistDoc::Left_Drag_Begin(const Point source, const Point target)
{
	Record_Begin(target);
	//printf("Left Button Drag Begin, [x,y] = [%u,%u]\n", target.x, target.y);
}

void ImpressionistDoc::Left_Drag_Move(const Point source, const Point target)
{
	Record_End(target);
	setDirAngle();
	Record_Begin(target);
	//printf("Left Button Drag Move, [x,y] = [%u,%u]\n", target.x, target.y);
}

void ImpressionistDoc::Right_Drag_Begin(const Point source, const Point target)
{
	Record_Begin(target);
	//printf("Right Button Drag Begin, [x,y] = [%u,%u]\n", target.x, target.y);
}

void ImpressionistDoc::Right_Drag_Move(const Point source, const Point target)
{
	Record_End(target);
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);
	glVertex2i(drag_start.x,drag_start.y);
	glVertex2i(drag_end.x,drag_end.y);
	glEnd();
	//printf("Right Button Drag Move, [x,y] = [%u,%u]\n", target.x, target.y);
}

void ImpressionistDoc::Right_Drag_End(const Point source, const Point target)
{
	Record_End(target);
	setDragAngle();
	//printf("Right Button Drag End, [x,y] = [%u,%u]\n", target.x, target.y);
}

void ImpressionistDoc::Record_Begin(const Point target)
{
	drag_start = target;
}

void ImpressionistDoc::Record_End(const Point target)
{
	drag_end = target;
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(iname, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth		= width;
	m_nPaintWidth	= width;
	m_nHeight		= height;
	m_nPaintHeight	= height;

	// release old storage
	delete [] m_ucBitmap;
	delete [] m_ucPainting;
	delete [] m_ucPreviewBackup;

	m_ucBitmap		= data;

	// allocate space for draw view
	m_ucPainting		= new unsigned char [width*height*3];
	m_ucPreviewBackup	= new unsigned char [width*height*3];
	memset(m_ucPainting, 0, width*height*3);
	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width*2, 
								height+25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);	
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);	
	m_pUI->m_paintView->refresh();

	// resize preview view
	m_pUI->m_PreviewWindow->resize(m_pUI->m_PreviewWindow->x(),
					m_pUI->m_PreviewWindow->y(),
					width,
					height);
	m_pUI->m_PreviewView->resizeWindow(width, height);
	m_pUI->m_PreviewView->refresh();
	return 1;
}


//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char *iname) 
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);

	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas() 
{

	// Release old storage
	if ( m_ucPainting ) 
	{
		delete [] m_ucPainting;

		// allocate space for draw view
		m_ucPainting	= new unsigned char [m_nPaintWidth*m_nPaintHeight*3];
		memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight*3);

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}
	
	return 0;
}

// Apply the filter specified by filter_kernel to the 
// each pixel in the source buffer and place the resulting
// pixel in the destination buffer.  

// This is called from the UI when the
// "preview" or "apply" button is pressed in the filter dialog.



/*
 *	INPUT: 
 *		sourceBuffer:		the original image buffer, 
 *		srcBufferWidth:		the width of the image buffer
 *		srcBufferHeight:	the height of the image buffer
 *							the buffer is arranged such that 
 *							origImg[3*(row*srcBufferWidth+column)+0], 
 *							origImg[3*(row*srcBufferWidth+column)+1], 
 *							origImg[3*(row*srcBufferWidth+column)+2]
 *							are R, G, B values for pixel at (column, row).
 *		destBuffer:			the image buffer to put the resulting
 *							image in.  It is always the same size
 *							as the source buffer.
 *
 *      filterKernel:		the 2D filter kernel,
 *		knlWidth:			the width of the kernel
 *		knlHeight:			the height of the kernel
 *
 *		divisor, offset:	each pixel after filtering should be
 *							divided by divisor and then added by offset
 */
void ImpressionistDoc::applyFilter( const unsigned char* sourceBuffer,
		int srcBufferWidth, int srcBufferHeight,
		unsigned char* destBuffer,
		const double *filterKernel, 
		int knlWidth, int knlHeight, 
		double divisor, double offset, int grayscale )
{
	// This needs to be implemented for image filtering to work.
	int Rtmp,Gtmp,Btmp,source_idx,source_idy,d_x,d_y;
	d_x = (knlWidth-1)/2;
	d_y = (knlHeight-1)/2;
	if(grayscale){
		unsigned char* grayscale;
		grayscale = new unsigned char[srcBufferWidth*srcBufferHeight];
		for(int i=0;i<srcBufferHeight;i++)
		for(int j=0;j<srcBufferWidth;j++){
			grayscale[i*srcBufferWidth+j]=0.299*sourceBuffer[3*(i*srcBufferWidth+j)]+0.587*sourceBuffer[3*(i*srcBufferWidth+j)+1]+0.114*sourceBuffer[3*(i*srcBufferWidth+j)+2];
		}
		for(int i=0;i<srcBufferHeight;i++)
		for(int j=0;j<srcBufferWidth;j++){
			Gtmp=0;
			for(int s=0;s<knlHeight;s++)
			for(int t=0;t<knlWidth;t++){
				source_idx = j+t-d_x;
				source_idy = i+s-d_x;
				round_index(&source_idx,srcBufferWidth-1);
				round_index(&source_idy,srcBufferHeight-1);
				Gtmp+=filterKernel[s*knlWidth+t]*grayscale[source_idy*srcBufferWidth+source_idx];
			}
			destBuffer[3*(i*srcBufferWidth+j)]=Gtmp/divisor+offset;
			destBuffer[3*(i*srcBufferWidth+j)+1]=Gtmp/divisor+offset;
			destBuffer[3*(i*srcBufferWidth+j)+2]=Gtmp/divisor+offset;
		}
		delete[] grayscale;
	}
	else{
		for(int i=0;i<srcBufferHeight;i++)
		for(int j=0;j<srcBufferWidth;j++){
			Rtmp=0;
			Gtmp=0;
			Btmp=0;
			for(int s=0;s<knlHeight;s++)
			for(int t=0;t<knlWidth;t++){
				source_idx = j+t-d_x;
				source_idy = i+s-d_y;
				round_index(&source_idx,srcBufferWidth-1);
				round_index(&source_idy,srcBufferHeight-1);
				Rtmp+=filterKernel[s*FLT_WIDTH+t]*sourceBuffer[3*(source_idy*srcBufferWidth+source_idx)];
				Gtmp+=filterKernel[s*FLT_WIDTH+t]*sourceBuffer[3*(source_idy*srcBufferWidth+source_idx)+1];
				Btmp+=filterKernel[s*FLT_WIDTH+t]*sourceBuffer[3*(source_idy*srcBufferWidth+source_idx)+2];
			}
			destBuffer[3*(i*srcBufferWidth+j)]=Rtmp/divisor+offset;
			destBuffer[3*(i*srcBufferWidth+j)+1]=Gtmp/divisor+offset;
			destBuffer[3*(i*srcBufferWidth+j)+2]=Btmp/divisor+offset;
		}
	}
}

void ImpressionistDoc::applySobelFilter( const unsigned char* sourceBuffer,
		int srcBufferWidth, int srcBufferHeight,
		unsigned char* destBuffer,
		const double *filterKernel, 
		int knlWidth, int knlHeight, 
		double divisor, double offset )
{
	// This needs to be implemented for image filtering to work.
	// Phase 2 of Sobel Combined, Sobel X result stored in dest Buffer
	int Gtmp,source_idx,source_idy,sob_x,sob_y,d_x,d_y;
	d_x = (knlWidth-1)/2;
	d_y = (knlHeight-1)/2;
	unsigned char* grayscale;
	grayscale = new unsigned char[srcBufferWidth*srcBufferHeight];
	for(int i=0;i<srcBufferHeight;i++)
	for(int j=0;j<srcBufferWidth;j++){
		grayscale[i*srcBufferWidth+j]=0.299*sourceBuffer[3*(i*srcBufferWidth+j)]+0.587*sourceBuffer[3*(i*srcBufferWidth+j)+1]+0.114*sourceBuffer[3*(i*srcBufferWidth+j)+2];
	}
	for(int i=0;i<srcBufferHeight;i++)
	for(int j=0;j<srcBufferWidth;j++){
		Gtmp=0;
		for(int s=0;s<knlHeight;s++)
		for(int t=0;t<knlWidth;t++){
			source_idx = j+t-d_x;
			source_idy = i+s-d_y;
			round_index(&source_idx,srcBufferWidth-1);
			round_index(&source_idy,srcBufferHeight-1);
			Gtmp+=filterKernel[s*FLT_WIDTH+t]*grayscale[source_idy*srcBufferWidth+source_idx];
		}
		for(int k=0;k<3;k++){
			sob_x=pow(destBuffer[3*(i*srcBufferWidth+j)+k],2);
			sob_y=pow(Gtmp/divisor+offset,2);
			destBuffer[3*(i*srcBufferWidth+j)+k]=sqrt((sob_x+sob_y)/2);
		}
	}
	delete[] grayscale;
}

void ImpressionistDoc::round_index(int* rhs, int bound){
	if(*rhs<0)
		*rhs = 0;
	if(*rhs>bound)
		*rhs = bound;
}

//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( int x, int y )
{
	if ( x < 0 ) 
		x = 0;
	else if ( x >= m_nWidth ) 
		x = m_nWidth-1;

	if ( y < 0 ) 
		y = 0;
	else if ( y >= m_nHeight ) 
		y = m_nHeight-1;

	return (GLubyte*)(m_ucBitmap + 3 * (y*m_nWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( const Point p )
{
	return GetOriginalPixel( p.x, p.y );
}



