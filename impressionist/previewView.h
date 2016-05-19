//
// previewview.h
//
// The header file for preview view of the input images
//

#ifndef PREVIEWVIEW_H
#define PREVIEWVIEW_H

#include <FL/Fl.H>

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/glu.h>

#include <stdlib.h>

#include "impBrush.h"

class ImpressionistDoc;

class PreviewView : public Fl_Gl_Window
{
public:
	PreviewView(int x, int y, int w, int h, const char *l);
	
	void draw();
	void refresh();

	void resizeWindow(int width, int height);

	ImpressionistDoc*	m_pDoc;

private:
	int	m_nWindowWidth, 
		m_nWindowHeight;

};

#endif

