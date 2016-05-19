//
// impressionistUI.h
//
// The header file for the UI part
//

#ifndef ImpressionistUI_h
#define ImpressionistUI_h

#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>		// FLTK file chooser
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Int_Input.H>

#include "impressionist.h"
#include "originalView.h"
#include "paintView.h"
#include "previewView.h"

#include "impBrush.h"

const int FLT_WIDTH = 7;
const int FLT_HEIGHT = 7;

enum
{
	FILTER_MEAN = 0,
	FILTER_WEIGHTED_MEAN,
	FILTER_SOBEL_X,
	FILTER_SOBEL_Y,
	FILTER_SOBEL_COMBINED,
	FILTER_LAPLACIAN,
	NUM_FILTER_TYPE // Make sure this stays at the end!
};

class ImpressionistUI {
public:
	ImpressionistUI();
	~ImpressionistUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*			m_menubar;
								
	PaintView*			m_paintView;
	OriginalView*			m_origView;

// for brush dialog
	Fl_Window*			m_brushDialog;
	Fl_Choice*			m_BrushTypeChoice;

// for filter dialog
	Fl_Window*			m_filterDialog;
	Fl_Choice*			m_FilterTypeChoice;
	Fl_Box*				m_FilterDescription;
	Fl_Button*          		m_UpdatFilterButton;
	Fl_Button*         		m_CancelFilterButton;
	Fl_Button*         		m_ApplyFilterButton;
	Fl_Button*          		m_PreviewFilterButton;
	Fl_Int_Input**			m_FilterWeight;
	Fl_Slider*			m_ColFilterSlider;
	Fl_Slider*			m_RowFilterSlider;
	Fl_Box*				m_SourceDescription;
	Fl_Round_Button*		m_OrigFilterTarget;
	Fl_Round_Button*		m_PaintFilterTarget;
	Fl_Window*			m_PreviewWindow;
	PreviewView*			m_PreviewView;

	Fl_Slider*			m_BrushSizeSlider;
	Fl_Slider*			m_BrushAlphaSlider;
	Fl_Slider*			m_BrushLinesizeSlider;
	Fl_Slider*			m_BrushAngleSlider;
	Fl_Box*				m_Buttontitle;
	Fl_Round_Button*		m_SliderButton;
	Fl_Round_Button*		m_MouseButton;
	Fl_Round_Button*		m_DirectionButton;
	Fl_Round_Button*		m_GradientButton;
	Fl_Button*         		m_ClearCanvasButton;

	// Member functions
	void				setDocument(ImpressionistDoc* doc);
	ImpressionistDoc*	getDocument();

	void				show();
	void				resize_windows(int w, int h);

	// Interface to get attribute

	int					getSize();
	float					getAlpha();
	int					getLinesize();
	int					getAngle();
	int				getColsize() {return m_nColumn;}
	int				getRowsize() {return m_nRow;}
	double				getScale() {return scale;}
	double				getOffset() {return offset;}
	double*				getfltKernel() {return fltKernel;}
	void				DirModeSet();
	void				DirModeUnset();
	void				IspaintviewSet();
	void				IspaintviewUnset();
	bool				getDirMode();
	bool				getIspaintview();
	void				setSize(int size);
	void				setAlpha(float alpha);
	void				setLinesize(int linesize);
	void				setAngle(int angle);
	void				setScale(double a) { scale = a;}
	void				setOffset(double a) { offset = a;}
	void				setfltKernel(int i,int value){ fltKernel[i] = value;}
	void				setColsize(int size){m_nColumn=size;}
	void				setRowsize(int size){m_nRow=size;}
	// Callbacks for the image filter dialogue (different from
	// the other callbacks because they aren't static)
	void				initFltDesignUI(void); // maybe not using
	void				updateFilter(void);

	void				cancelFilter(void);
	void				applyFilter(void);
	void				previewFilter(void);
	void				setFilterType(int type);
	int				getFilterType(void) {return m_nFilterType;}
	void				reset_filter(void);


private:
	ImpressionistDoc*	m_pDoc;		// pointer to document to communicate with the document

	// All attributes here
	int		m_nSize;
	float		m_nAlpha;
	int		m_nLinesize;
	int		m_nAngle;
	bool		m_nDirMode;
	int		m_nColumn;
	int		m_nRow;
	bool		m_nIspaintview;
	int		m_nFilterType;
	// These attributes are set by the filter kernel UI
	double fltKernel[FLT_WIDTH*FLT_HEIGHT];		//the kernel of the image filter
	double scale,offset;						//the divisor and offset parameter, 
												//applied after convolving to transform 
												//the filtered image into the range [0,255]


	// Static class members
	static Fl_Menu_Item		menuitems[];
	static Fl_Menu_Item		brushTypeMenu[NUM_BRUSH_TYPE+1];
	static Fl_Menu_Item		filterTypeMenu[NUM_FILTER_TYPE+1];

	static ImpressionistUI*	whoami(Fl_Menu_* o);

	// All callbacks here.  Callbacks are declared 
	// static
	static void	cb_load_image(Fl_Menu_* o, void* v);
	static void	cb_save_image(Fl_Menu_* o, void* v);
	static void	cb_brushes(Fl_Menu_* o, void* v);
	static void	cb_filter_kernel(Fl_Menu_* o, void* v);
	static void	cb_clear_canvas(Fl_Menu_* o, void* v);
	static void	cb_exit(Fl_Menu_* o, void* v);
	static void	cb_about(Fl_Menu_* o, void* v);
	static void	cb_brushChoice(Fl_Widget* o, void* v);
	static void	cb_clear_canvas_button(Fl_Widget* o, void* v);
	static void	cb_sizeSlides(Fl_Widget* o, void* v);
	static void	cb_alphaSlides(Fl_Widget* o, void* v);
	static void	cb_linesizeSlides(Fl_Widget* o, void* v);
	static void	cb_angleSlides(Fl_Widget* o, void* v);
	static void	cb_SliderButton(Fl_Widget* o, void* v);
	static void	cb_MouseButton(Fl_Widget* o, void* v);
	static void	cb_DirectionButton(Fl_Widget* o, void* v);
	static void	cb_GradientButton(Fl_Widget* o, void* v);
	
	static void	cb_filterChoice(Fl_Widget* o, void* v);
	static void	cb_update_filter(Fl_Widget* o, void* v);
	static void	cb_cancel_filter(Fl_Widget* o, void* v);
	static void	cb_apply_filter(Fl_Widget* o, void* v);
	static void	cb_preview_filter(Fl_Widget* o, void* v);
	static void	cb_ColFilterSlides(Fl_Widget* o, void* v);
	static void	cb_RowFilterSlides(Fl_Widget* o, void* v);
	static void	cb_OrigFilterTarget(Fl_Widget* o, void* v);
	static void	cb_PaintFilterTarget(Fl_Widget* o, void* v);

};

#endif

