// 
// impressionistDoc.h
//
// header file for Doc 
//

#ifndef ImpressionistDoc_h
#define ImpressionistDoc_h

#include "impressionist.h"
#include "bitmap.h"

class ImpressionistUI;

class ImpressionistDoc 
{
public:
	ImpressionistDoc();

	void	setUI(ImpressionistUI* ui);		// Assign the UI to use

	int		loadImage(char *iname);			// called by the UI to load image
	int		saveImage(char *iname);			// called by the UI to save image


	int     clearCanvas();                  // called by the UI to clear the drawing canvas

	void applyFilter( const unsigned char* sourceBuffer, //The filter kernel
		int srcBufferWidth, int srcBufferHeight,
		unsigned char* destBuffer,
		const double *filterKernel, 
		int knlWidth, int knlHeight, 
		double divisor, double offset, int grayscale );

	void applySobelFilter( const unsigned char* sourceBuffer, //The filter kernel
		int srcBufferWidth, int srcBufferHeight,
		unsigned char* destBuffer,
		const double *filterKernel, 
		int knlWidth, int knlHeight, 
		double divisor, double offset );

	void	setBrushType(int type);			// called by the UI to set the brushType
	int		getSize();						// get the UI size
	float		getAlpha();						// get the UI alpha
	int		getLinesize();						// get the UI line size
	int		getAngle();						// get the UI angle
	int		getDirAngle();
	void	setSize(int size);				// set the UI size
	void	setAlpha(int alpha);				// set the UI alpha
	void	setLinesize(int linesize);			// set the UI line size
	void	setAngle(int angle);				// set the UI Angle
	char*	getImageName();					// get the current image name
	void	Left_Drag_Begin(const Point source, const Point target);
	void	Left_Drag_Move(const Point source, const Point target);
	void	Right_Drag_Begin(const Point source, const Point target);
	void	Right_Drag_Move(const Point source, const Point target);
	void	Right_Drag_End(const Point source, const Point target);
	void	Record_Begin(const Point target);
	void	Record_End(const Point target);
	void	setDirAngle();
	void	setDragAngle();
	void	round_index(int* rhs,int bound);

	unsigned char*	m_ucPreviewBackup;

// Attributes
public:
	// Dimensions of original window.
	int				m_nWidth, 
					m_nHeight;
	// Dimensions of the paint window.
	int				m_nPaintWidth, 
					m_nPaintHeight;	
	// Bitmaps for original image and painting.
	unsigned char*	m_ucBitmap;
	unsigned char*	m_ucPainting;
	
	// Used by the filtering code so that we can
	// preview filters before applying.
	//unsigned char*	m_ucPreviewBackup;



	// The current active brush.
	ImpBrush*			m_pCurrentBrush;	
	// Size of the brush.
	int m_nSize;
	float m_nAlpha;
	int m_nLinesize;
	int m_nAngle;				
	int m_nDirAngle;
	Point			drag_start,drag_end;

	ImpressionistUI*	m_pUI;

// Operations
public:
	// Get the color of the original picture at the specified coord
	GLubyte* GetOriginalPixel( int x, int y );   
	// Get the color of the original picture at the specified point	
	GLubyte* GetOriginalPixel( const Point p );



private:
	char			m_imageName[256];
};

extern void MessageBox(char *message);

#endif
