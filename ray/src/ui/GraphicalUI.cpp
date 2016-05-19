//
// GraphicalUI.cpp
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <thread>
#include <assert.h>

#ifndef COMMAND_LINE_ONLY

#include <FL/fl_ask.H>
#include "debuggingView.h"

#include "GraphicalUI.h"
#include "CubeMapChooser.h"
#include "../RayTracer.h"

#define MAX_INTERVAL 500

#ifdef _WIN32
#define print sprintf_s
#else
#define print sprintf
#endif

bool GraphicalUI::stopTrace = false;
bool GraphicalUI::doneTrace = true;
GraphicalUI* GraphicalUI::pUI = NULL;
char* GraphicalUI::traceWindowLabel = "Raytraced Image";
bool TraceUI::m_debug = false;

//------------------------------------- Help Functions --------------------------------------------
GraphicalUI* GraphicalUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ((GraphicalUI*)(o->parent()->user_data()));
}

//--------------------------------- Callback Functions --------------------------------------------
void GraphicalUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	pUI = whoami(o);

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			print(buf, "Ray <%s>", newfile);
			stopTracing();	// terminate the previous rendering
		} else print(buf, "Ray <Not Loaded>");

		pUI->m_mainWindow->label(buf);
		pUI->m_debuggingWindow->m_debuggingView->setDirty();

		if( lastFile != 0 && strcmp(newfile, lastFile) != 0 )
			pUI->m_debuggingWindow->m_debuggingView->resetCamera();

		pUI->m_debuggingWindow->redraw();
	}
}

void GraphicalUI::cb_load_cubemap(Fl_Menu_* o, void* v) 
{
	pUI = whoami(o);
	pUI->m_cubeMapChooser->setCaller(pUI);
	pUI->m_cubeMapChooser->show();
}

void GraphicalUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	pUI = whoami(o);

	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void GraphicalUI::cb_exit(Fl_Menu_* o, void* v)
{
	pUI = whoami(o);

	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_debuggingWindow->hide();
	pUI->m_cubeMapChooser->hide();
	TraceUI::m_debug = false;
}

void GraphicalUI::cb_exit2(Fl_Widget* o, void* v) 
{
	pUI = (GraphicalUI *)(o->user_data());

	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_debuggingWindow->hide();
	pUI->m_cubeMapChooser->hide();
	TraceUI::m_debug = false;
}

void GraphicalUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project for CS384g.");
}

void GraphicalUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_blockSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_Blocksize=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_thresholdSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_Threshold=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());

	// terminate the rendering so we don't get crashes
	stopTracing();

	pUI->m_nSize=int(((Fl_Slider *)o)->value());
	int width = (int)(pUI->getSize());
	int height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow(width, height);
}

void GraphicalUI::cb_refreshSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->refreshInterval=clock_t(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_threadSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_Threads=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_aaSamplesSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_aaSampleWidth=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_aaThreshSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_aaThreshold=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_treeDepthSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_KdDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_leafSizeSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_targetleafsize=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_filterSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_nFilterWidth=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_aaButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	if(pUI->m_antialias==true) pUI->m_antialias = false;
	else pUI->m_antialias = true;
}

void GraphicalUI::cb_kdButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	if(pUI->m_kdtree==true) pUI->m_kdtree = false;
	else pUI->m_kdtree = true;
}

void GraphicalUI::cb_cmButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	if(pUI->m_usingCubeMap==true) pUI->m_usingCubeMap = false;
	else pUI->m_usingCubeMap = true;
}

void GraphicalUI::cb_ssButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	if(pUI->m_smoothshade==true) pUI->m_smoothshade = false;
	else pUI->m_smoothshade = true;
}

void GraphicalUI::cb_shButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	if(pUI->m_shadows==true) pUI->m_shadows = false;
	else pUI->m_shadows = true;
}

void GraphicalUI::cb_bfButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	if(pUI->m_backfacecull==true) pUI->m_backfacecull = false;
	else pUI->m_backfacecull = true;
}

void GraphicalUI::cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_displayDebuggingInfo = (((Fl_Check_Button*)o)->value() == 1);
	if (pUI->m_displayDebuggingInfo)
	  {
	    pUI->m_debuggingWindow->show();
	    pUI->m_debug = true;
	  }
	else
	  {
	    pUI->m_debuggingWindow->hide();
	    pUI->m_debug = false;
	  }
}

void GraphicalUI::cb_render(Fl_Widget* o, void* v) {

	char buffer[256];
	char aabuffer[256];

	pUI = (GraphicalUI*)(o->user_data());
	doneTrace = stopTrace = false;
	if (pUI->raytracer->sceneLoaded())
	  {
		int width = pUI->getSize();
		int height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		int origPixels = width * height;
		pUI->m_traceGlWindow->resizeWindow(width, height);
		pUI->m_traceGlWindow->show();
		pUI->raytracer->traceSetup(width, height);

		// Save the window label
                const char *old_label = pUI->m_traceGlWindow->label();

		clock_t now, prev;
		now = prev = clock();
		clock_t intervalMS = pUI->refreshInterval * 100;
		int thread_num = (int)pUI->getThread()-1;
		std::thread tt[thread_num];
		int blocksize = pUI->getBlocksize();
		for(int y = 0; y<height/blocksize;y++){
			for(int x = 0; x<width/blocksize;x++){
				if (stopTrace){
					for(int i=0;i<thread_num;i++)
						tt[i].join();
					break;
				}
				// check for input and refresh view every so often while tracing
				now = clock();
				if ((now - prev)/CLOCKS_PER_SEC * 1000 >= intervalMS)
				  {
				    prev = now;
				    sprintf(buffer, "(%d%%) %s", (int)((double)y*blocksize / (double)height * 100.0), old_label);
				    pUI->m_traceGlWindow->label(buffer);
				    pUI->m_traceGlWindow->refresh();
				    Fl::check();
				    if (Fl::damage()) { Fl::flush(); }
				  }
				// look for input and refresh window
				int thread_id = x%(thread_num+1);
				if(x==(width/blocksize-1))
					pUI->raytracer->blocktracePixel(x,y);
				else if(thread_id<thread_num)
					tt[thread_id] = std::thread(&RayTracer::blocktracePixel, pUI->raytracer, x, y);
				else
					pUI->raytracer->blocktracePixel(x,y);
				if(x%(thread_num+1)==thread_num||x==(width/blocksize-1))
				for(int i=0;i<thread_id;i++){
					tt[i].join();
				}
				pUI->m_debuggingWindow->m_debuggingView->setDirty();
			}
			if (stopTrace) {
				for(int i=0;i<thread_num;i++)
					tt[i].join();
				break;
			};
		}
		if(width%blocksize!=0||height%blocksize!=0){
			for(int y = 0;y<height;y++)
			for(int x = 0;x<width;x++)
				if(x>=int(width/blocksize)*blocksize||y>=int(height/blocksize)*blocksize)
					pUI->raytracer->tracePixel(x,y);
		}
		//for (int y = 0; y < height; y++)
		//  {
		//    for (int x = 0; x < width; x++)
		//      {
		//	if (stopTrace) break;
		//	// check for input and refresh view every so often while tracing
		//	now = clock();
		//	if ((now - prev)/CLOCKS_PER_SEC * 1000 >= intervalMS)
		//	  {
		//	    prev = now;
		//	    sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
		//	    pUI->m_traceGlWindow->label(buffer);
		//	    pUI->m_traceGlWindow->refresh();
		//	    Fl::check();
		//	    if (Fl::damage()) { Fl::flush(); }
		//	  }
		//	// look for input and refresh window
		//	int thread_id = x%(thread_num+1);
		//	if(thread_id<thread_num)
		//		tt[thread_id] = std::thread(&RayTracer::tracePixel, pUI->raytracer, x, y);
		//	else
		//		pUI->raytracer->tracePixel(x,y);
		//	if(x%(thread_num+1)==7||x==width-1)
		//	for(int i=0;i<thread_num;i++){
		//		tt[i].join();
		//	}
		//	pUI->m_debuggingWindow->m_debuggingView->setDirty();
		//	//tt[thread_id].join();
		//      }
		//    if (stopTrace) break;
		//  }
	
		if(pUI->Antialias()){
			for(int y = 0;y<height;y++){
				for(int x=0;x<width;x++){
					if (stopTrace){
						for(int i=0;i<thread_num;i++)
							tt[i].join();
						break;
					}
					now = clock();
					if((now-prev)/CLOCKS_PER_SEC * 1000 >= intervalMS){
						prev = now;
			    			sprintf(aabuffer, "AntiAlias: (%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
				    		pUI->m_traceGlWindow->label(aabuffer);
			    			pUI->m_traceGlWindow->refresh();
			    			Fl::check();
			    			if (Fl::damage()) { Fl::flush(); }
					}
					bool needaa = false;
					if(x<width-1&&y<height-1&&x>0&&y>0){
						double mag_mxmy = (pUI->raytracer->buffer[3*(x-1+(y-1)*width)]+pUI->raytracer->buffer[3*(x-1+(y-1)*width)+1]+pUI->raytracer->buffer[3*(x-1+(y-1)*width)+2])/255.0;
						double mag_pxmy = (pUI->raytracer->buffer[3*(x+1+(y-1)*width)]+pUI->raytracer->buffer[3*(x+1+(y-1)*width)+1]+pUI->raytracer->buffer[3*(x+1+(y-1)*width)+2])/255.0;
						double mag_mxpy = (pUI->raytracer->buffer[3*(x-1+(y+1)*width)]+pUI->raytracer->buffer[3*(x-1+(y+1)*width)+1]+pUI->raytracer->buffer[3*(x-1+(y+1)*width)+2])/255.0;
						double mag_pxpy = (pUI->raytracer->buffer[3*(x+1+(y+1)*width)]+pUI->raytracer->buffer[3*(x+1+(y+1)*width)+1]+pUI->raytracer->buffer[3*(x+1+(y+1)*width)+2])/255.0;
						double threshold = pUI->m_aaThreshSlider->value()*0.001;
						if(fabs(mag_mxmy-mag_mxpy)>threshold) needaa = true;
						else if(fabs(mag_mxmy-mag_pxmy)>threshold) needaa = true;
						else if(fabs(mag_mxmy-mag_pxpy)>threshold) needaa = true;
						else if(fabs(mag_pxmy-mag_mxpy)>threshold) needaa = true;
						else if(fabs(mag_pxmy-mag_pxpy)>threshold) needaa = true;
						else if(fabs(mag_mxpy-mag_pxpy)>threshold) needaa = true;
					}
					int thread_id = x%(thread_num+1);
					if(x==width-1)
						pUI->raytracer->AntiAlias(x,y,needaa);
					else if(thread_id<thread_num)
						tt[thread_id] = std::thread(&RayTracer::AntiAlias, pUI->raytracer, x, y ,needaa);
					else
						pUI->raytracer->AntiAlias(x,y,needaa);
					if(x%(thread_num+1)==thread_num||x==width-1)
					for(int i=0;i<thread_id;i++){
						tt[i].join();
					}
					//pUI->raytracer->AntiAlias(x,y,needaa);
					pUI->m_debuggingWindow->m_debuggingView->setDirty();
				}
				if (stopTrace){
					for(int i=0;i<thread_num;i++)
						tt[i].join();
					break;
				}
			}
			std::memcpy(pUI->raytracer->buffer,pUI->raytracer->aabuffer,pUI->raytracer->buffer_width*pUI->raytracer->buffer_height*3);
		}
		doneTrace = true;
		stopTrace = false;
		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);
		pUI->m_traceGlWindow->refresh();
	  }
}

void GraphicalUI::cb_stop(Fl_Widget* o, void* v)
{
	pUI = (GraphicalUI*)(o->user_data());
	stopTracing();
}

int GraphicalUI::run()
{
	Fl::visual(FL_DOUBLE|FL_INDEX);

	m_mainWindow->show();

	return Fl::run();
}

void GraphicalUI::alert( const string& msg )
{
	fl_alert( "%s", msg.c_str() );
}

void GraphicalUI::setRayTracer(RayTracer *tracer)
{
	TraceUI::setRayTracer(tracer);
	m_traceGlWindow->setRayTracer(tracer);
	m_debuggingWindow->m_debuggingView->setRayTracer(tracer);
}

// menu definition
Fl_Menu_Item GraphicalUI::menuitems[] = {
	{ "&File", 0, 0, 0, FL_SUBMENU },
	{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)GraphicalUI::cb_load_scene },
	{ "&Load Cubemap...",	FL_ALT + 'c', (Fl_Callback *)GraphicalUI::cb_load_cubemap },
	{ "&Save Image...", FL_ALT + 's', (Fl_Callback *)GraphicalUI::cb_save_image },
	{ "&Exit", FL_ALT + 'e', (Fl_Callback *)GraphicalUI::cb_exit },
	{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
	{ "&About",	FL_ALT + 'a', (Fl_Callback *)GraphicalUI::cb_about },
	{ 0 },

	{ 0 }
};

void GraphicalUI::stopTracing()
{
	stopTrace = true;
}

GraphicalUI::GraphicalUI() : refreshInterval(10) {
	// init.
	m_mainWindow = new Fl_Window(100, 40, 450, 459, "Ray <Not Loaded>");
	m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
	// install menu bar
	m_menubar = new Fl_Menu_Bar(0, 0, 440, 25);
	m_menubar->menu(menuitems);

	// set up "render" button
	m_renderButton = new Fl_Button(360, 37, 70, 25, "&Render");
	m_renderButton->user_data((void*)(this));
	m_renderButton->callback(cb_render);

	// set up "stop" button
	m_stopButton = new Fl_Button(360, 65, 70, 25, "&Stop");
	m_stopButton->user_data((void*)(this));
	m_stopButton->callback(cb_stop);

	// install depth slider
	m_depthSlider = new Fl_Value_Slider(10, 40, 180, 20, "Recursion Depth");
	m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_depthSlider->type(FL_HOR_NICE_SLIDER);
	m_depthSlider->labelfont(FL_COURIER);
	m_depthSlider->labelsize(12);
	m_depthSlider->minimum(0);
	m_depthSlider->maximum(10);
	m_depthSlider->step(1);
	m_depthSlider->value(m_nDepth);
	m_depthSlider->align(FL_ALIGN_RIGHT);
	m_depthSlider->callback(cb_depthSlides);
	
	// install blocksize slider
	m_blockSlider = new Fl_Value_Slider(10, 65, 180, 20, "Blocksize");
	m_blockSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_blockSlider->type(FL_HOR_NICE_SLIDER);
	m_blockSlider->labelfont(FL_COURIER);
	m_blockSlider->labelsize(12);
	m_blockSlider->minimum(2);
	m_blockSlider->maximum(64);
	m_blockSlider->step(1);
	m_blockSlider->value(m_Blocksize);
	m_blockSlider->align(FL_ALIGN_RIGHT);
	m_blockSlider->callback(cb_blockSlides);
	
	// install threshold slider
	m_thresholdSlider = new Fl_Value_Slider(10, 90, 180, 20, "Threshold (x 0.001)");
	m_thresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_thresholdSlider->type(FL_HOR_NICE_SLIDER);
	m_thresholdSlider->labelfont(FL_COURIER);
	m_thresholdSlider->labelsize(12);
	m_thresholdSlider->minimum(0);
	m_thresholdSlider->maximum(1000);
	m_thresholdSlider->step(1);
	m_thresholdSlider->deactivate();
	m_thresholdSlider->value(m_Threshold);
	m_thresholdSlider->align(FL_ALIGN_RIGHT);
	m_thresholdSlider->callback(cb_thresholdSlides);

	// install size slider
	m_sizeSlider = new Fl_Value_Slider(10, 115, 180, 20, "Screen Size");
	m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_sizeSlider->type(FL_HOR_NICE_SLIDER);
	m_sizeSlider->labelfont(FL_COURIER);
	m_sizeSlider->labelsize(12);
	m_sizeSlider->minimum(64);
	m_sizeSlider->maximum(1024);
	m_sizeSlider->step(2);
	m_sizeSlider->value(m_nSize);
	m_sizeSlider->align(FL_ALIGN_RIGHT);
	m_sizeSlider->callback(cb_sizeSlides);

	// install refresh interval slider
	m_refreshSlider = new Fl_Value_Slider(10, 140, 180, 20, "Screen Refresh Interval (0.1 sec)");
	m_refreshSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_refreshSlider->type(FL_HOR_NICE_SLIDER);
	m_refreshSlider->labelfont(FL_COURIER);
	m_refreshSlider->labelsize(12);
	m_refreshSlider->minimum(1);
	m_refreshSlider->maximum(300);
	m_refreshSlider->step(1);
	m_refreshSlider->value(refreshInterval);
	m_refreshSlider->align(FL_ALIGN_RIGHT);
	m_refreshSlider->callback(cb_refreshSlides);

	// install thread slider
	m_threadSlider = new Fl_Value_Slider(10, 165, 180, 20, "Threads");
	m_threadSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_threadSlider->type(FL_HOR_NICE_SLIDER);
	m_threadSlider->labelfont(FL_COURIER);
	m_threadSlider->labelsize(12);
	m_threadSlider->minimum(1);
	m_threadSlider->maximum(32);
	m_threadSlider->step(1);
	m_threadSlider->value(m_Threads);
	m_threadSlider->align(FL_ALIGN_RIGHT);
	m_threadSlider->callback(cb_threadSlides);

	// install refresh interval slider
	m_aaSamplesSlider = new Fl_Value_Slider(120, 205, 180, 20, "Pixel Samples");
	m_aaSamplesSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_aaSamplesSlider->type(FL_HOR_NICE_SLIDER);
	m_aaSamplesSlider->labelfont(FL_COURIER);
	m_aaSamplesSlider->labelsize(12);
	m_aaSamplesSlider->minimum(1);
	m_aaSamplesSlider->maximum(8);
	m_aaSamplesSlider->step(1);
	m_aaSamplesSlider->value(m_aaSampleWidth);
	m_aaSamplesSlider->align(FL_ALIGN_RIGHT);
	m_aaSamplesSlider->callback(cb_aaSamplesSlides);

	// install antialas check button
	m_aaCheckButton = new Fl_Check_Button(10, 225, 20, 20, "Antialias");
	m_aaCheckButton->user_data((void*)(this));	// record self to be used by static callback functions
	m_aaCheckButton->labelfont(FL_COURIER);
	m_aaCheckButton->value(m_antialias);
	m_aaCheckButton->callback(cb_aaButton);

	// install aa threshold slider
	m_aaThreshSlider = new Fl_Value_Slider(120, 245, 180, 20, "Supersample\nThreshold (x 0.001)");
	m_aaThreshSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_aaThreshSlider->type(FL_HOR_NICE_SLIDER);
	m_aaThreshSlider->labelfont(FL_COURIER);
	m_aaThreshSlider->labelsize(12);
	m_aaThreshSlider->minimum(0);
	m_aaThreshSlider->maximum(1000);
	m_aaThreshSlider->step(1);
	m_aaThreshSlider->value(m_aaThreshold);
	m_aaThreshSlider->align(FL_ALIGN_RIGHT);
	m_aaThreshSlider->callback(cb_aaThreshSlides);

	// install tree depth slider
	m_treeDepthSlider = new Fl_Value_Slider(120, 285, 180, 20, "Max Depth");
	m_treeDepthSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_treeDepthSlider->type(FL_HOR_NICE_SLIDER);
	m_treeDepthSlider->labelfont(FL_COURIER);
	m_treeDepthSlider->labelsize(12);
	m_treeDepthSlider->minimum(1);
	m_treeDepthSlider->maximum(30);
	m_treeDepthSlider->step(1);
	m_treeDepthSlider->value(m_KdDepth);
	m_treeDepthSlider->align(FL_ALIGN_RIGHT);
	m_treeDepthSlider->callback(cb_treeDepthSlides);

	// install kdtree button
	m_kdCheckButton = new Fl_Check_Button(10, 305, 20, 20, "K-d Tree");
	m_kdCheckButton->user_data((void*)(this));	// record self to be used by static callback functions
	m_kdCheckButton->labelfont(FL_COURIER);
	m_kdCheckButton->value(m_kdtree);
	m_kdCheckButton->callback(cb_kdButton);

	// install leaf size slider
	m_leafSizeSlider = new Fl_Value_Slider(120, 325, 180, 20, "Target Leaf Size");
	m_leafSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_leafSizeSlider->type(FL_HOR_NICE_SLIDER);
	m_leafSizeSlider->labelfont(FL_COURIER);
	m_leafSizeSlider->labelsize(12);
	m_leafSizeSlider->minimum(1);
	m_leafSizeSlider->maximum(100);
	m_leafSizeSlider->step(1);
	m_leafSizeSlider->value(m_targetleafsize);
	m_leafSizeSlider->align(FL_ALIGN_RIGHT);
	m_leafSizeSlider->callback(cb_leafSizeSlides);
	
	// install cubemap button
	m_cubeMapCheckButton = new Fl_Check_Button(10, 365, 20, 20, "CubeMap");
	m_cubeMapCheckButton->user_data((void*)(this));	// record self to be used by static callback functions
	m_cubeMapCheckButton->labelfont(FL_COURIER);
	m_cubeMapCheckButton->value(m_usingCubeMap);
	m_cubeMapCheckButton->deactivate();
	m_cubeMapCheckButton->callback(cb_cmButton);

	// install filter slider
	m_filterSlider = new Fl_Value_Slider(120, 365, 180, 20, "Filter Width");
	m_filterSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_filterSlider->type(FL_HOR_NICE_SLIDER);
	m_filterSlider->labelfont(FL_COURIER);
	m_filterSlider->labelsize(12);
	m_filterSlider->minimum(1);
	m_filterSlider->maximum(17);
	m_filterSlider->step(1);
	m_filterSlider->value(m_nFilterWidth);
	m_filterSlider->deactivate();
	m_filterSlider->align(FL_ALIGN_RIGHT);
	m_filterSlider->callback(cb_filterSlides);
	
	// install smooth shade button
	m_ssCheckButton = new Fl_Check_Button(10, 395, 20, 20, "Smoothshade");
	m_ssCheckButton->user_data((void*)(this));	// record self to be used by static callback functions
	m_ssCheckButton->labelfont(FL_COURIER);
	m_ssCheckButton->value(m_smoothshade);
	m_ssCheckButton->callback(cb_ssButton);

	// install shadow button
	m_shCheckButton = new Fl_Check_Button(145, 395, 20, 20, "Shadow");
	m_shCheckButton->user_data((void*)(this));	// record self to be used by static callback functions
	m_shCheckButton->labelfont(FL_COURIER);
	m_shCheckButton->value(m_shadows);
	m_shCheckButton->callback(cb_shButton);

	// install backface cull
	m_bfCheckButton = new Fl_Check_Button(250, 395, 20, 20, "Backface Cull");
	m_bfCheckButton->user_data((void*)(this));	// record self to be used by static callback functions
	m_bfCheckButton->labelfont(FL_COURIER);
	m_bfCheckButton->value(m_backfacecull);
	m_bfCheckButton->callback(cb_bfButton);
	
	// set up debugging display checkbox
	m_debuggingDisplayCheckButton = new Fl_Check_Button(10, 425, 140, 20, "Debugging display");
	m_debuggingDisplayCheckButton->user_data((void*)(this));
	m_debuggingDisplayCheckButton->callback(cb_debuggingDisplayCheckButton);
	m_debuggingDisplayCheckButton->value(m_displayDebuggingInfo);

	m_mainWindow->callback(cb_exit2);
	m_mainWindow->when(FL_HIDE);
	m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, traceWindowLabel);
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);

	// debugging view
	m_debuggingWindow = new DebuggingWindow();
	m_cubeMapChooser = new CubeMapChooser();
}

#endif
