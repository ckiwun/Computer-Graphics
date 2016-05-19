//
// impressionistUI.cpp
//
// The user interface part for the program.
//


#include <FL/fl_ask.H>

#include <cstdlib>
#include <algorithm>
#include <iostream>

#include <math.h>

#include "impressionistUI.h"
#include "impressionistDoc.h"

/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the 
project.  You can copy and paste these into your code and then change them to 
make them look how you want.  Descriptions for all of the widgets here can be found 
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------
	
	//----To install a window--------------------------
	Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
		myWindow->user_data((void*)(this));	// record self to be used by static callback functions
		
		// install menu bar
		myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
		Fl_Menu_Item ImpressionistUI::myMenuItems[] = {
			{ "&File",		0, 0, 0, FL_SUBMENU },
				{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load },
				{ "&Save...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save }.
				{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
				{ 0 },
			{ "&Edit",		0, 0, 0, FL_SUBMENU },
				{ "&Copy",FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_copy, (void *)COPY },
				{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)ImpressionistUI::cb_cut, (void *)CUT },
				{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)ImpressionistUI::cb_paste, (void *)PASTE },
				{ 0 },
			{ "&Help",		0, 0, 0, FL_SUBMENU },
				{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
				{ 0 },
			{ 0 }
		};
		myMenubar->menu(myMenuItems);
    myWindow->end();

	//----The window callback--------------------------
	// One of the callbacks
	void ImpressionistUI::cb_load(Fl_Menu_* o, void* v) 
	{	
		ImpressionistDoc *pDoc=whoami(o)->getDocument();

		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
		if (newfile != NULL) {
			pDoc->loadImage(newfile);
		}
	}


//------------Slider---------------------------------------

	//----To install a slider--------------------------
	Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
	mySlider->user_data((void*)(this));	// record self to be used by static callback functions
	mySlider->type(FL_HOR_NICE_SLIDER);
    mySlider->labelfont(FL_COURIER);
    mySlider->labelsize(12);
	mySlider->minimum(1);
	mySlider->maximum(40);
	mySlider->step(1);
	mySlider->value(m_nMyValue);
	mySlider->align(FL_ALIGN_RIGHT);
	mySlider->callback(cb_MyValueSlides);

	//----The slider callback--------------------------
	void ImpressionistUI::cb_MyValueSlides(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
	}
	

//------------Choice---------------------------------------
	
	//----To install a choice--------------------------
	Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
	myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
	Fl_Menu_Item ImpressionistUI::myChoiceMenu[3+1] = {
	  {"one",FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)ONE},
	  {"two",FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)TWO},
	  {"three",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)THREE},
	  {0}
	};
	myChoice->menu(myChoiceMenu);
	myChoice->callback(cb_myChoice);
	
	//-----The choice callback-------------------------
	void ImpressionistUI::cb_myChoice(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
		ImpressionistDoc* pDoc=pUI->getDocument();

		int type=(int)v;

		pDoc->setMyType(type);
	}


//------------Button---------------------------------------

	//---To install a button---------------------------
	Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
	myButton->user_data((void*)(this));   // record self to be used by static callback functions
	myButton->callback(cb_myButton);

	//---The button callback---------------------------
	void ImpressionistUI::cb_myButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
		ImpressionistDoc* pDoc = pUI->getDocument();
		pDoc->startPainting();
	}


//---------Light Button------------------------------------
	
	//---To install a light button---------------------
	Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
	myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
	myLightButton->callback(cb_myLightButton);

	//---The light button callback---------------------
	void ImpressionistUI::cb_myLightButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI *pUI=((ImpressionistUI*)(o->user_data()));

		if (pUI->myBool==TRUE) pUI->myBool=FALSE;
		else pUI->myBool=TRUE;
	}

//----------Int Input--------------------------------------

    //---To install an int input-----------------------
	Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
	myInput->user_data((void*)(this));   // record self to be used by static callback functions
	myInput->callback(cb_myInput);

	//---The int input callback------------------------
	void ImpressionistUI::cb_myInput(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
	}

//------------------------------------------------------------------------------------------------
*/

//------------------------------------- Help Functions --------------------------------------------

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI* ImpressionistUI::whoami(Fl_Menu_* o)	
{
	return ( (ImpressionistUI*)(o->parent()->user_data()) );
}


//--------------------------------- Callback Functions --------------------------------------------

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_load_image(Fl_Menu_* o, void* v) 
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
	if (newfile != NULL) {
		pDoc->loadImage(newfile);
	}
}


//------------------------------------------------------------------
// Brings up a file chooser and then saves the painted image
// This is called by the UI when the save image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Save File?", "*.bmp", "save.bmp" );
	if (newfile != NULL) {
		pDoc->saveImage(newfile);
	}
}

//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the brushes menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_brushes(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_brushDialog->show();
}
//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the filter menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_filter_kernel(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_filterDialog->show();
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas(Fl_Menu_* o, void* v)
{
	ImpressionistDoc* pDoc=whoami(o)->getDocument();

	pDoc->clearCanvas();
}

//------------------------------------------------------------
// Causes the Impressionist program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_exit(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_mainWindow->hide();
	whoami(o)->m_brushDialog->hide();
	whoami(o)->m_filterDialog->hide();

}



//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void ImpressionistUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("Impressionist FLTK version 1.1.9 for CS 384G, Fall 2008");
}

//------- UI should keep track of the current for all the controls for answering the query from Doc ---------
//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush 
// choice.  
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_brushChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();

	//	int type=(int)v;
	long long tmp = reinterpret_cast<long long>(v);
	int type = static_cast<int>(tmp);

	pDoc->setBrushType(type);
}

void ImpressionistUI::cb_filterChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();

	//	int type=(int)v;
	long long tmp = reinterpret_cast<long long>(v);
	int type = static_cast<int>(tmp);

	pUI->setFilterType(type);
	if(!(type==FILTER_WEIGHTED_MEAN||type==FILTER_MEAN)){
		pUI->reset_filter();
	}
	if(!(type==FILTER_WEIGHTED_MEAN))
		pUI->updateFilter();
}
void ImpressionistUI::cb_update_filter(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();
	if(!(pUI->getFilterType()==FILTER_WEIGHTED_MEAN||pUI->getFilterType()==FILTER_MEAN))
		pUI->reset_filter();
	pUI->updateFilter();
	//Update Preview
	if(pUI->m_PaintFilterTarget->value()==1){//PaintView was selected as source
		if(pUI->getFilterType()==FILTER_MEAN||pUI->getFilterType()==FILTER_WEIGHTED_MEAN)
		pDoc->applyFilter(	pDoc->m_ucPainting,
					pDoc->m_nPaintWidth,
					pDoc->m_nPaintHeight,
					pDoc->m_ucPreviewBackup,
					pUI->getfltKernel(),
					pUI->getColsize(),
					pUI->getRowsize(),
					pUI->getScale(),
					pUI->getOffset(),
					0
					);
		else
		pDoc->applyFilter(	pDoc->m_ucPainting,
					pDoc->m_nPaintWidth,
					pDoc->m_nPaintHeight,
					pDoc->m_ucPreviewBackup,
					pUI->getfltKernel(),
					pUI->getColsize(),
					pUI->getRowsize(),
					pUI->getScale(),
					pUI->getOffset(),
					1
					);
	}
	else {
		if(pUI->getFilterType()==FILTER_MEAN||pUI->getFilterType()==FILTER_WEIGHTED_MEAN)
		pDoc->applyFilter(	pDoc->m_ucBitmap,
					pDoc->m_nWidth,
					pDoc->m_nHeight,
					pDoc->m_ucPreviewBackup,
					pUI->getfltKernel(),
					pUI->getColsize(),
					pUI->getRowsize(),
					pUI->getScale(),
					pUI->getOffset(),
					0
					);
		else	
		pDoc->applyFilter(	pDoc->m_ucBitmap,
					pDoc->m_nWidth,
					pDoc->m_nHeight,
					pDoc->m_ucPreviewBackup,
					pUI->getfltKernel(),
					pUI->getColsize(),
					pUI->getRowsize(),
					pUI->getScale(),
					pUI->getOffset(),
					1
					);
	}
	if(pUI->getFilterType()==FILTER_SOBEL_COMBINED){
		pUI->setFilterType(FILTER_SOBEL_Y);
		pUI->updateFilter();
		if(pUI->getIspaintview())//PaintView was selected as source
			pDoc->applySobelFilter(	pDoc->m_ucPainting,
						pDoc->m_nPaintWidth,
						pDoc->m_nPaintHeight,
						pDoc->m_ucPreviewBackup,
						pUI->getfltKernel(),
						pUI->getColsize(),
						pUI->getRowsize(),
						pUI->getScale(),
						pUI->getOffset()
						);
		else
			pDoc->applySobelFilter(	pDoc->m_ucBitmap,
						pDoc->m_nWidth,
						pDoc->m_nHeight,
						pDoc->m_ucPreviewBackup,
						pUI->getfltKernel(),
						pUI->getColsize(),
						pUI->getRowsize(),
						pUI->getScale(),
						pUI->getOffset()
						);
		pUI->setFilterType(FILTER_SOBEL_COMBINED);
	}
}

void ImpressionistUI::cb_cancel_filter(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();
	pUI->m_PreviewWindow->hide();
	pUI->m_filterDialog->hide();
	pUI->cancelFilter();
}

void ImpressionistUI::cb_apply_filter(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();
	
	pUI->cb_update_filter(o,v);
	pUI->m_filterDialog->hide();
	pUI->m_PreviewWindow->hide();
	memcpy(pDoc->m_ucPainting, pDoc->m_ucPreviewBackup, 3*pDoc->m_nWidth*pDoc->m_nHeight);
	pUI->m_paintView->refresh();
	pUI->applyFilter();
}

void ImpressionistUI::cb_preview_filter(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();
	pUI->cb_update_filter(o,v);
	pUI->m_PreviewView->refresh();
	//pUI->m_PreviewView->draw();
	pUI->m_PreviewWindow->show();
	pUI->previewFilter();
}
//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

	pDoc->clearCanvas();
}


//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_alphaSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nAlpha=float( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_linesizeSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nLinesize=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_angleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nAngle=int( ((Fl_Slider *)o)->value() ) ;
}
// Chooses the ways to decide angle value by round button call back
void ImpressionistUI::cb_SliderButton(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pUI->m_SliderButton->set();
	pUI->m_MouseButton->clear();
	pUI->m_DirectionButton->clear();
	pUI->m_GradientButton->clear();
	pUI->DirModeUnset();
}

void ImpressionistUI::cb_MouseButton(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pUI->m_SliderButton->clear();
	pUI->m_MouseButton->set();
	pUI->m_DirectionButton->clear();
	pUI->m_GradientButton->clear();
	pUI->DirModeUnset();
}

void ImpressionistUI::cb_DirectionButton(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pUI->m_SliderButton->clear();
	pUI->m_MouseButton->clear();
	pUI->m_DirectionButton->set();
	pUI->m_GradientButton->clear();
	pUI->DirModeSet();
}

void ImpressionistUI::cb_GradientButton(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pUI->m_SliderButton->clear();
	pUI->m_MouseButton->clear();
	pUI->m_DirectionButton->clear();
	pUI->m_GradientButton->set();
	pUI->DirModeUnset();
}

void ImpressionistUI::cb_ColFilterSlides(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	
	for(int i=0;i<FLT_HEIGHT;i++)
	for(int j=0;j<FLT_WIDTH;j++){
	pUI->m_FilterWeight[i*FLT_WIDTH+j]->activate();
	pUI->m_FilterWeight[i*FLT_WIDTH+j]->value("1");
	pUI->m_FilterWeight[i*FLT_WIDTH+j]->color(FL_BACKGROUND2_COLOR);
	if(i>pUI->m_RowFilterSlider->value()-1||j>pUI->m_ColFilterSlider->value()-1){
		pUI->m_FilterWeight[i*FLT_WIDTH+j]->deactivate();
		pUI->m_FilterWeight[i*FLT_WIDTH+j]->value("0");
	}
	if(i==(int(pUI->m_RowFilterSlider->value())-1)/2&&j==(int(pUI->m_ColFilterSlider->value())-1)/2)
		pUI->m_FilterWeight[i*FLT_WIDTH+j]->color(FL_YELLOW);
	}
	pUI->m_filterDialog->redraw();
	pUI->m_nColumn=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_RowFilterSlides(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	
	for(int i=0;i<FLT_HEIGHT;i++)
	for(int j=0;j<FLT_WIDTH;j++){
	pUI->m_FilterWeight[i*FLT_WIDTH+j]->activate();
	pUI->m_FilterWeight[i*FLT_WIDTH+j]->value("1");
	pUI->m_FilterWeight[i*FLT_WIDTH+j]->color(FL_BACKGROUND2_COLOR);
	if(i>pUI->m_RowFilterSlider->value()-1||j>pUI->m_ColFilterSlider->value()-1){
		pUI->m_FilterWeight[i*FLT_WIDTH+j]->deactivate();
		pUI->m_FilterWeight[i*FLT_WIDTH+j]->value("0");
	}
	if(i==(int(pUI->m_RowFilterSlider->value())-1)/2&&j==(int(pUI->m_ColFilterSlider->value())-1)/2)
		pUI->m_FilterWeight[i*FLT_WIDTH+j]->color(FL_YELLOW);
	}
	pUI->m_filterDialog->redraw();
	pUI->m_nRow=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_OrigFilterTarget(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pUI->m_OrigFilterTarget->set();
	pUI->m_PaintFilterTarget->clear();
}

void ImpressionistUI::cb_PaintFilterTarget(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();
	pUI->m_OrigFilterTarget->clear();
	pUI->m_PaintFilterTarget->set();
}

void ImpressionistUI::updateFilter(void)
{
	int tmp,pos_weight,neg_weight;
	if(getFilterType()!=FILTER_WEIGHTED_MEAN)
	for(int i = 0; i<FLT_HEIGHT*FLT_WIDTH;i++){
		setfltKernel(i,0);
		m_FilterWeight[i]->value("0");
	}
	pos_weight=neg_weight=0;
	switch(getFilterType()){
		case FILTER_MEAN:
		for(int i = 0; i<getRowsize();i++)
		for(int j = 0; j<getColsize();j++){
			setfltKernel(i*FLT_WIDTH+j,1);
			m_FilterWeight[i*FLT_WIDTH+j]->value("1");
			pos_weight++;
		}
		setScale(pos_weight);
		setOffset(0);
		break;
		case FILTER_WEIGHTED_MEAN:
		for(int i = 0; i<getRowsize();i++)
		for(int j = 0; j<getColsize();j++){
			tmp = atoi(m_FilterWeight[i*FLT_WIDTH+j]->value());
			setfltKernel(i*FLT_WIDTH+j,tmp);
			if(tmp>=0)
				pos_weight+=tmp;
			else
				neg_weight+=tmp;
		}
		setScale(pos_weight-neg_weight);
		setOffset(-255*neg_weight/(pos_weight-neg_weight));
		break;
		case FILTER_SOBEL_X:
			setfltKernel(0,-1);
			m_FilterWeight[0]->value("-1");
			setfltKernel(1,0);
			m_FilterWeight[1]->value("0");
			setfltKernel(2,1);
			m_FilterWeight[2]->value("1");
			setfltKernel(7,-2);
			m_FilterWeight[7]->value("-2");
			setfltKernel(8,0);
			m_FilterWeight[8]->value("0");
			setfltKernel(9,2);
			m_FilterWeight[9]->value("2");
			setfltKernel(14,-1);
			m_FilterWeight[14]->value("-1");
			setfltKernel(15,0);
			m_FilterWeight[15]->value("0");
			setfltKernel(16,1);
			m_FilterWeight[16]->value("1");
		setScale(8);
		setOffset(127.5);
		break;
		case FILTER_SOBEL_Y:
			setfltKernel(0,-1);
			m_FilterWeight[0]->value("-1");
			setfltKernel(1,-2);
			m_FilterWeight[1]->value("-2");
			setfltKernel(2,-1);
			m_FilterWeight[2]->value("-1");
			setfltKernel(7,0);
			m_FilterWeight[7]->value("0");
			setfltKernel(8,0);
			m_FilterWeight[8]->value("0");
			setfltKernel(9,0);
			m_FilterWeight[9]->value("0");
			setfltKernel(14,1);
			m_FilterWeight[14]->value("1");
			setfltKernel(15,2);
			m_FilterWeight[15]->value("2");
			setfltKernel(16,1);
			m_FilterWeight[16]->value("1");
		setScale(8);
		setOffset(127.5);
		break;
		case FILTER_SOBEL_COMBINED:
			setfltKernel(0,-1);
			m_FilterWeight[0]->value("-1");
			setfltKernel(1,0);
			m_FilterWeight[1]->value("0");
			setfltKernel(2,1);
			m_FilterWeight[2]->value("1");
			setfltKernel(7,-2);
			m_FilterWeight[7]->value("-2");
			setfltKernel(8,0);
			m_FilterWeight[8]->value("0");
			setfltKernel(9,2);
			m_FilterWeight[9]->value("2");
			setfltKernel(14,-1);
			m_FilterWeight[14]->value("-1");
			setfltKernel(15,0);
			m_FilterWeight[15]->value("0");
			setfltKernel(16,1);
			m_FilterWeight[16]->value("1");
		setOffset(127.5);
		break;
		case FILTER_LAPLACIAN:
			setfltKernel(0,0);
			m_FilterWeight[0]->value("0");
			setfltKernel(1,1);
			m_FilterWeight[1]->value("1");
			setfltKernel(2,0);
			m_FilterWeight[2]->value("0");
			setfltKernel(7,1);
			m_FilterWeight[7]->value("1");
			setfltKernel(8,-4);
			m_FilterWeight[8]->value("-4");
			setfltKernel(9,1);
			m_FilterWeight[9]->value("1");
			setfltKernel(14,0);
			m_FilterWeight[14]->value("0");
			setfltKernel(15,1);
			m_FilterWeight[15]->value("1");
			setfltKernel(16,0);
			m_FilterWeight[16]->value("0");
		setScale(8);
		setOffset(127.5);
		break;
	}
}

void ImpressionistUI::cancelFilter(void)
{
}

void ImpressionistUI::applyFilter(void)
{
}

void ImpressionistUI::previewFilter(void)
{
}

void ImpressionistUI::setFilterType(int type)
{
	m_nFilterType = type;
}

void ImpressionistUI::reset_filter()
{
	for(int i=0;i<FLT_HEIGHT;i++)
	for(int j=0;j<FLT_WIDTH;j++){
	m_FilterWeight[i*FLT_WIDTH+j]->activate();
	m_FilterWeight[i*FLT_WIDTH+j]->color(FL_BACKGROUND2_COLOR);
	if(i>2||j>2)
		m_FilterWeight[i*FLT_WIDTH+j]->deactivate();
	if(i==1&&j==1)
		m_FilterWeight[i*FLT_WIDTH+j]->color(FL_YELLOW);
	}
	m_filterDialog->redraw();
	switch(getFilterType()){
		case  FILTER_MEAN:
		break;
		case  FILTER_WEIGHTED_MEAN:
		break;
		case  FILTER_SOBEL_X:
			m_ColFilterSlider->value(3);
			m_RowFilterSlider->value(3);
			setColsize(3);
			setRowsize(3);
		break;
		case  FILTER_SOBEL_Y:
			m_ColFilterSlider->value(3);
			m_RowFilterSlider->value(3);
			setColsize(3);
			setRowsize(3);
		break;
		case  FILTER_SOBEL_COMBINED:
			m_ColFilterSlider->value(3);
			m_RowFilterSlider->value(3);
			setColsize(3);
			setRowsize(3);
		break;
		case  FILTER_LAPLACIAN:
			m_ColFilterSlider->value(3);
			m_RowFilterSlider->value(3);
			setColsize(3);
			setRowsize(3);
		break;
	}
}
//---------------------------------- per instance functions --------------------------------------

//------------------------------------------------
// Return the ImpressionistDoc used
//------------------------------------------------
ImpressionistDoc* ImpressionistUI::getDocument()
{
	return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void ImpressionistUI::show() {
	m_mainWindow->show();
	m_paintView->show();
	m_origView->show();
}

//------------------------------------------------
// Change the paint and original window sizes to 
// w by h
//------------------------------------------------
void ImpressionistUI::resize_windows(int w, int h) {
	m_paintView->size(w,h);
	m_origView->size(w,h);
}

//------------------------------------------------ 
// Set the ImpressionistDoc used by the UI to 
// communicate with the brushes 
//------------------------------------------------
void ImpressionistUI::setDocument(ImpressionistDoc* doc)
{
	m_pDoc = doc;

	m_origView->m_pDoc = doc;
	m_paintView->m_pDoc = doc;
	m_PreviewView->m_pDoc = doc;
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getSize()
{
	return m_nSize;
}

float ImpressionistUI::getAlpha()
{
	return m_nAlpha;
}

int ImpressionistUI::getLinesize()
{
	return m_nLinesize;
}

int ImpressionistUI::getAngle()
{
	return m_nAngle;
}

bool ImpressionistUI::getDirMode()
{
	return m_nDirMode;
}

bool ImpressionistUI::getIspaintview()
{
	return m_nIspaintview;
}

void ImpressionistUI::DirModeSet()
{
	m_nDirMode=1;
}

void ImpressionistUI::DirModeUnset()
{
	m_nDirMode=0;
}

void ImpressionistUI::IspaintviewSet()
{
	m_nIspaintview=1;
}

void ImpressionistUI::IspaintviewUnset()
{
	m_nIspaintview=0;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setSize( int size )
{
	m_nSize=size;

	if (size<=100) 
		m_BrushSizeSlider->value(m_nSize);
}

void ImpressionistUI::setAlpha( float alpha )
{
	m_nAlpha=alpha;

	if (alpha<=1) 
		m_BrushAlphaSlider->value(m_nAlpha);
}

void ImpressionistUI::setLinesize( int linesize )
{
	m_nLinesize=linesize;

	if (linesize<=10) 
		m_BrushLinesizeSlider->value(m_nLinesize);
}


void ImpressionistUI::setAngle( int angle )
{
	m_nAngle=angle;

	if (angle<=360) 
		m_BrushAngleSlider->value(m_nAngle);
}

// Main menu definition
Fl_Menu_Item ImpressionistUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Image...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load_image },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save_image },
		{ "&Brushes...",	FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_brushes }, 
		{ "&Filter...",	FL_ALT + 'f', (Fl_Callback *)ImpressionistUI::cb_filter_kernel }, 
		{ "&Clear Canvas", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_clear_canvas, 0, FL_MENU_DIVIDER },
		
		{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
		{ 0 },

	{ 0 }
};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::brushTypeMenu[NUM_BRUSH_TYPE+1] = {
  {"Points",			FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_POINTS},
  {"Lines",				FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_LINES},
  {"Circles",			FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_CIRCLES},
  {"Scattered Points",	FL_ALT+'q', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_POINTS},
  {"Scattered Lines",	FL_ALT+'m', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_LINES},
  {"Scattered Circles",	FL_ALT+'d', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_CIRCLES},
  {0}
};

// Filter choice menu definition
Fl_Menu_Item ImpressionistUI::filterTypeMenu[NUM_FILTER_TYPE+1] = {
  {"Mean",			FL_ALT+'m', (Fl_Callback *)ImpressionistUI::cb_filterChoice, (void *)FILTER_MEAN},
  {"Weighted Mean",	FL_ALT+'w', (Fl_Callback *)ImpressionistUI::cb_filterChoice, (void *)FILTER_WEIGHTED_MEAN},
  {"Sobel X",		FL_ALT+'x', (Fl_Callback *)ImpressionistUI::cb_filterChoice, (void *)FILTER_SOBEL_X},
  {"Sobel Y",		FL_ALT+'y', (Fl_Callback *)ImpressionistUI::cb_filterChoice, (void *)FILTER_SOBEL_Y},
  {"Sobel Combined",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_filterChoice, (void *)FILTER_SOBEL_COMBINED},
  {"Laplacian",		FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_filterChoice, (void *)FILTER_LAPLACIAN},
  {0}
};


//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
ImpressionistUI::ImpressionistUI() {
	// Create the main window
	m_mainWindow = new Fl_Window(600, 300, "Impressionist");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
		m_menubar->menu(menuitems);

		// Create a group that will hold two sub windows inside the main
		// window
		Fl_Group* group = new Fl_Group(0, 25, 600, 275);

			// install paint view window
			m_paintView = new PaintView(300, 25, 300, 275, "This is the paint view");//0jon
			m_paintView->box(FL_DOWN_FRAME);

			// install original view window
			m_origView = new OriginalView(0, 25, 300, 275, "This is the orig view");//300jon
			m_origView->box(FL_DOWN_FRAME);
			m_origView->deactivate();

		group->end();
		Fl_Group::current()->resizable(group);
    m_mainWindow->end();
	
	// init values

	m_nSize = 10;
	m_nAlpha = 1;
	m_nLinesize = 1;
	m_nAngle = 0;
	DirModeUnset();
	IspaintviewSet();
	m_nColumn = 3;
	m_nRow = 3;
	m_nFilterType = FILTER_MEAN;
	m_FilterWeight = new Fl_Int_Input*[FLT_HEIGHT*FLT_WIDTH];

	// brush dialog definition
	m_brushDialog = new Fl_Window(400, 325, "Brush Dialog");
		// Add a brush type choice to the dialog
		m_BrushTypeChoice = new Fl_Choice(50,10,150,25,"&Brush");
		m_BrushTypeChoice->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushTypeChoice->menu(brushTypeMenu);
		m_BrushTypeChoice->callback(cb_brushChoice);

		m_ClearCanvasButton = new Fl_Button(240,10,150,25,"&Clear Canvas");
		m_ClearCanvasButton->user_data((void*)(this));
		m_ClearCanvasButton->callback(cb_clear_canvas_button);


		// Add brush size slider to the dialog 
		m_BrushSizeSlider = new Fl_Value_Slider(10, 80, 300, 20, "Size");
		m_BrushSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushSizeSlider->type(FL_HOR_NICE_SLIDER);
	        m_BrushSizeSlider->labelfont(FL_COURIER);
        	m_BrushSizeSlider->labelsize(12);
		m_BrushSizeSlider->minimum(1);
		m_BrushSizeSlider->maximum(100);
		m_BrushSizeSlider->step(1);
		m_BrushSizeSlider->value(m_nSize);
		m_BrushSizeSlider->align(FL_ALIGN_RIGHT);
		m_BrushSizeSlider->callback(cb_sizeSlides);


		m_BrushAlphaSlider = new Fl_Value_Slider(10, 100, 300, 20, "Alpha");
		m_BrushAlphaSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushAlphaSlider->type(FL_HOR_NICE_SLIDER);
	        m_BrushAlphaSlider->labelfont(FL_COURIER);
        	m_BrushAlphaSlider->labelsize(12);
		m_BrushAlphaSlider->minimum(0);
		m_BrushAlphaSlider->maximum(1);
		m_BrushAlphaSlider->step(0.01);
		m_BrushAlphaSlider->value(m_nAlpha);
		m_BrushAlphaSlider->align(FL_ALIGN_RIGHT);
		m_BrushAlphaSlider->callback(cb_alphaSlides);

		
		m_BrushLinesizeSlider = new Fl_Value_Slider(10, 120, 300, 20, "Line Size");
		m_BrushLinesizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushLinesizeSlider->type(FL_HOR_NICE_SLIDER);
	        m_BrushLinesizeSlider->labelfont(FL_COURIER);
        	m_BrushLinesizeSlider->labelsize(12);
		m_BrushLinesizeSlider->minimum(1);
		m_BrushLinesizeSlider->maximum(10);
		m_BrushLinesizeSlider->step(1);
		m_BrushLinesizeSlider->value(m_nLinesize);
		m_BrushLinesizeSlider->align(FL_ALIGN_RIGHT);
		m_BrushLinesizeSlider->callback(cb_linesizeSlides);

		
		m_BrushAngleSlider = new Fl_Value_Slider(10, 160, 300, 20, "Angle");
		m_BrushAngleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushAngleSlider->type(FL_HOR_NICE_SLIDER);
	        m_BrushAngleSlider->labelfont(FL_COURIER);
        	m_BrushAngleSlider->labelsize(12);
		m_BrushAngleSlider->minimum(0);
		m_BrushAngleSlider->maximum(360);
		m_BrushAngleSlider->step(1);
		m_BrushAngleSlider->value(m_nAngle);
		m_BrushAngleSlider->align(FL_ALIGN_RIGHT);
		m_BrushAngleSlider->callback(cb_angleSlides);


		m_Buttontitle = new Fl_Box(10, 200, 300, 20, "Direction Choice");
	        m_Buttontitle->labelfont(FL_COURIER);
        	m_Buttontitle->labelsize(16);
		m_Buttontitle->box(FL_NO_BOX);


		m_SliderButton = new Fl_Round_Button(10,220,20,20,"&Slider");
		m_SliderButton->set();
		m_SliderButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_SliderButton->callback(cb_SliderButton);


		m_MouseButton = new Fl_Round_Button(10,240,20,20,"&Mouse");
		m_MouseButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_MouseButton->callback(cb_MouseButton);


		m_DirectionButton = new Fl_Round_Button(10,260,20,20,"&Direction");
		m_DirectionButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_DirectionButton->callback(cb_DirectionButton);


		m_GradientButton = new Fl_Round_Button(10,280,20,20,"&Gradient");
		m_GradientButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_GradientButton->callback(cb_GradientButton);


	m_brushDialog->end();	
	

	// Filter Dialogue
	m_filterDialog = new Fl_Window(400, 325, "Filter Dialog");
		m_FilterTypeChoice = new Fl_Choice(50,10,150,25,"&Filter");
		m_FilterTypeChoice->user_data((void*)(this));	// record self to be used by static callback functions
		m_FilterTypeChoice->menu(filterTypeMenu);
		m_FilterTypeChoice->callback(cb_filterChoice);
	
		m_FilterDescription = new Fl_Box(20, 45, 320, 20, "For Weight Mean use only           Kernel Size");
	        m_FilterDescription->labelfont(FL_COURIER);
        	m_FilterDescription->labelsize(12);
		m_FilterDescription->box(FL_NO_BOX);
		for(int i=0;i<FLT_HEIGHT;i++)
		for(int j=0;j<FLT_WIDTH;j++){
			m_FilterWeight[i*FLT_WIDTH+j] = new Fl_Int_Input(10+30*j, 65+30*i, 25, 25, "");
			m_FilterWeight[i*FLT_WIDTH+j]->user_data((void*)(this));
			m_FilterWeight[i*FLT_WIDTH+j]->value("0");
			if(i<3&&j<3)
				m_FilterWeight[i*FLT_WIDTH+j]->value("1");
			if(i>2||j>2)
				m_FilterWeight[i*FLT_WIDTH+j]->deactivate();
			if(i==1&&j==1)
				m_FilterWeight[i*FLT_WIDTH+j]->color(FL_YELLOW);
		}

		m_ColFilterSlider = new Fl_Value_Slider(225, 80, 160, 20, "Column size");
		m_ColFilterSlider->user_data((void*)(this));
		m_ColFilterSlider->type(FL_HOR_NICE_SLIDER);
	        m_ColFilterSlider->labelfont(FL_COURIER);
        	m_ColFilterSlider->labelsize(12);
		m_ColFilterSlider->minimum(3);
		m_ColFilterSlider->maximum(7);
		m_ColFilterSlider->step(1);
		m_ColFilterSlider->value(m_nColumn);
		m_ColFilterSlider->align(FL_ALIGN_TOP);
		m_ColFilterSlider->callback(cb_ColFilterSlides);
		
		m_RowFilterSlider = new Fl_Value_Slider(225, 120, 160, 20, "Row size");
		m_RowFilterSlider->user_data((void*)(this));
		m_RowFilterSlider->type(FL_HOR_NICE_SLIDER);
	        m_RowFilterSlider->labelfont(FL_COURIER);
        	m_RowFilterSlider->labelsize(12);
		m_RowFilterSlider->minimum(3);
		m_RowFilterSlider->maximum(7);
		m_RowFilterSlider->step(1);
		m_RowFilterSlider->value(m_nRow);
		m_RowFilterSlider->align(FL_ALIGN_TOP);
		m_RowFilterSlider->callback(cb_RowFilterSlides);

		m_SourceDescription = new Fl_Box(225, 150, 160, 20, "Select Filter Source");
	        m_SourceDescription->labelfont(FL_COURIER);
        	m_SourceDescription->labelsize(12);
		m_SourceDescription->box(FL_NO_BOX);

		m_OrigFilterTarget = new Fl_Round_Button(225,200,20,20,"&OriginView");
		m_OrigFilterTarget->user_data((void*)(this));   // record self to be used by static callback functions
		m_OrigFilterTarget->callback(cb_OrigFilterTarget);

		m_PaintFilterTarget = new Fl_Round_Button(225,180,20,20,"&PaintView");
		m_PaintFilterTarget->set();
		m_PaintFilterTarget->user_data((void*)(this));   // record self to be used by static callback functions
		m_PaintFilterTarget->callback(cb_PaintFilterTarget);

		m_UpdatFilterButton = new Fl_Button(225,240,150,25,"&Update");
		m_UpdatFilterButton->user_data((void*)(this));
		m_UpdatFilterButton->callback(cb_update_filter);
		
		m_CancelFilterButton = new Fl_Button(225,285,150,25,"&Cancel");
		m_CancelFilterButton->user_data((void*)(this));
		m_CancelFilterButton->callback(cb_cancel_filter);
		
		m_ApplyFilterButton = new Fl_Button(25,285,150,25,"&Apply");
		m_ApplyFilterButton->user_data((void*)(this));
		m_ApplyFilterButton->callback(cb_apply_filter);
		
		m_PreviewFilterButton = new Fl_Button(225,10,150,25,"&Preview");
		m_PreviewFilterButton->user_data((void*)(this));
		m_PreviewFilterButton->callback(cb_preview_filter);

	m_filterDialog->end();

	m_PreviewWindow = new Fl_Window(300, 300, "Preview Window");
		m_PreviewWindow->user_data((void*)(this));
		m_PreviewView = new PreviewView(0, 0, 300, 300, "This is the preview view");
		m_PreviewView->box(FL_DOWN_FRAME);
		m_PreviewView->deactivate();

	m_PreviewWindow->end();
}

ImpressionistUI::~ImpressionistUI()
{
}
