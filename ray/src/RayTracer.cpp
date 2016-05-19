// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <assert.h>
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

template <typename T>
class KdNode;

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates (x,y),
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

Vec3d RayTracer::trace(double x, double y)
{
  // Clear out the ray cache in the scene for debugging purposes,
  if (TraceUI::m_debug) scene->intersectCache.clear();
  ray r(Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY);
  scene->getCamera().rayThrough(x,y,r);
  Vec3d ret = traceRay(r, traceUI->getDepth());
  ret.clamp();
  return ret;
}

Vec3d RayTracer::tracePixel(int i, int j)
{
	Vec3d col(0,0,0);
	if( ! sceneLoaded() ) return col;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
	//cout << "tracepixel " << i << "," << j << endl;
	col = trace(x, y);

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
	return col;
}

Vec3d RayTracer::blocktracePixel(int i, int j)
{
	int blocksize = traceUI->getBlocksize();
	int width = traceUI->getSize();
	Vec3d c0 = tracePixel(i*blocksize,j*blocksize);
	Vec3d c1 = tracePixel(i*blocksize+blocksize-1,j*blocksize);
	Vec3d c2 = tracePixel(i*blocksize,j*blocksize+blocksize-1);
	Vec3d c3 = tracePixel(i*blocksize+blocksize-1,j*blocksize+blocksize-1);
	bool cmp_01 = fabs(c0.length()-c1.length())==0;
	bool cmp_02 = fabs(c0.length()-c2.length())==0;
	bool cmp_03 = fabs(c0.length()-c3.length())==0;
	bool cmp_12 = fabs(c1.length()-c2.length())==0;
	bool cmp_13 = fabs(c1.length()-c3.length())==0;
	bool cmp_23 = fabs(c2.length()-c3.length())==0;
	if(cmp_01&&cmp_02&&cmp_03&&cmp_12&&cmp_13&&cmp_23){
		unsigned char* pixel_orig = buffer+(i*blocksize+j*blocksize*width)*3;
		for(int k=0;k<blocksize;k++)
		for(int t=0;t<blocksize;t++){
			unsigned char* pixel = buffer+(i*blocksize+t+(j*blocksize+k)*width)*3;
			pixel[0] = pixel_orig[0];
			pixel[1] = pixel_orig[1];
			pixel[2] = pixel_orig[2];
		}
	}
	else{
		for(int k=0;k<blocksize;k++)
		for(int t=0;t<blocksize;t++){
			tracePixel(i*blocksize+t,j*blocksize+k);
		}
	}
}

Vec3d RayTracer::AntiAlias(int i, int j,bool needaa)
{
	//if(needaa) cout << "antialias " << i << "," << j << endl;
	Vec3d col(0,0,0);
	unsigned char *pixel = aabuffer + ( i + j * buffer_width ) * 3;
	unsigned char *pixel_orig = buffer + ( i + j * buffer_width ) * 3;
	int samplewidth = traceUI->getSampleWidth();
	if(needaa){
		for(int k=0;k<samplewidth;k++)
		for(int t=0;t<samplewidth;t++){
			double x = (double(i-0.5)+(double)t/(double)(samplewidth-1))/double(buffer_width);
			double y = (double(j-0.5)+(double)k/(double)(samplewidth-1))/double(buffer_height);
			//cout << x << " , " << y << endl;
			col += trace(x,y);
		}
		col /= (double)(samplewidth*samplewidth);
	
		pixel[0] = (int)( 255.0 * col[0]);
		pixel[1] = (int)( 255.0 * col[1]);
		pixel[2] = (int)( 255.0 * col[2]);
	}
	else{
		pixel[0] = pixel_orig[0];
		pixel[1] = pixel_orig[1];
		pixel[2] = pixel_orig[2];
	}
	return col;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
Vec3d RayTracer::traceRay(ray& r, int depth)
{
	isect i;
	Vec3d colorC;
	double n_i,n_t;
	if(scene->kd_intersect(r,i)){
	//if(scene->intersect(r, i)) {

		// An intersection occurred!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
		const Material& m = i.getMaterial();
		Vec3d q_normal = i.N;
		bool entering = true;
		double cos_in_ang = (-r.getDirection())*i.N;
		bool TIR = false;
		if(cos_in_ang<0){ // leaving
			entering = false;
			q_normal = -q_normal;
			i.N = -i.N;
			cos_in_ang = -cos_in_ang;
		}
		if(depth>0){
			if(!m.kt(i).iszero()){
				if(entering){
					n_i = 1; // assuming at least one substrate is air
					n_t = m.index(i);
				}
				else  { 
					n_i = m.index(i);
					n_t = 1;
				}
				if(n_i>1){
					double crit_ang = asin(1/n_i);
					double in_ang = acos(cos_in_ang); // need make sure both are unit vectors
					if(in_ang>crit_ang) TIR = true;
				}
				if(!TIR){
					double index_ratio = n_i / n_t;
					double sin2_tr_ang = index_ratio*index_ratio*(1-cos_in_ang*cos_in_ang);
					Vec3d RefrDir = index_ratio*r.getDirection()+
							(index_ratio*cos_in_ang-sqrt(1-sin2_tr_ang))*q_normal;
					RefrDir.normalize();
					ray RefrRay = ray(r.at(i.t),RefrDir,ray::REFRACTION);
					colorC += prod(m.kt(i),traceRay(RefrRay,depth-1));
				}
			}
			if(!m.kr(i).iszero()){ // early ray termination
				Vec3d ReflDir = 2*cos_in_ang*q_normal+r.getDirection();
				//ReflDir.normalize();
				ray ReflRay = ray(r.at(i.t),ReflDir,ray::REFLECTION);
				if(TIR) colorC += traceRay(ReflRay,depth-1);
				else colorC += prod(m.kr(i),traceRay(ReflRay,depth-1));
			}
		}
		colorC += m.shade(scene, r, i);
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		//colorC = Vec3d(0.0, 0.0, 0.0);
		if(!traceUI->UsingCubeMap()){
			colorC = Vec3d(0.0,0.0,0.0);
		}
		else{
			int main_axis = 0;
			if(	(fabs(r.getDirection()[1]) > fabs(r.getDirection()[0])) && 
				(fabs(r.getDirection()[1]) > fabs(r.getDirection()[2])) )
				main_axis = 1;
			if(	(fabs(r.getDirection()[2]) > fabs(r.getDirection()[0])) && 
				(fabs(r.getDirection()[2]) > fabs(r.getDirection()[1])) )
				main_axis = 2;
			switch(main_axis){
				case 0:
					if(r.getDirection()[0]>=0)
						colorC = getCubeMap()->getXposMap()->getMappedValue(Vec2d(1.0f-((r.getDirection()[2])/(r.getDirection()[0])+1.0f)*0.5f,((r.getDirection()[1])/(r.getDirection()[0])+1.0f)*0.5f));
					else
						colorC = getCubeMap()->getXnegMap()->getMappedValue(Vec2d(1.0f-((r.getDirection()[2])/(r.getDirection()[0])+1.0f)*0.5f,1.0f-((r.getDirection()[1])/(r.getDirection()[0])+1.0f)*0.5f));
					break;
				case 1:
					if(r.getDirection()[1]>=0)
						colorC = getCubeMap()->getYposMap()->getMappedValue(Vec2d(((r.getDirection()[0])/(r.getDirection()[1])+1.0f)*0.5f,1.0f-((r.getDirection()[2])/(r.getDirection()[1])+1.0f)*0.5f));
					else
						colorC = getCubeMap()->getYnegMap()->getMappedValue(Vec2d(1.0f-((r.getDirection()[0])/(r.getDirection()[1])+1.0f)*0.5f,((r.getDirection()[2])/(r.getDirection()[1])+1.0f)*0.5f));
					break;
				case 2:
					if(r.getDirection()[2]>=0)
						colorC = getCubeMap()->getZposMap()->getMappedValue(Vec2d(((r.getDirection()[0])/(r.getDirection()[2])+1.0f)*0.5f,((r.getDirection()[1])/(r.getDirection()[2])+1.0f)*0.5f));
					else
						colorC = getCubeMap()->getZnegMap()->getMappedValue(Vec2d(((r.getDirection()[0])/(r.getDirection()[2])+1.0f)*0.5f,1.0f-((r.getDirection()[1])/(r.getDirection()[2])+1.0f)*0.5f));
					break;
			}
		}
	}
	return colorC;
}

RayTracer::RayTracer()
	: scene(0), buffer(0), buffer_width(256), buffer_height(256), m_bBufferReady(false), aabuffer(0)
{}

RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
	delete [] aabuffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn ) {
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos ) path = ".";
	else path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
		scene->buildKdTree(traceUI->getKdDepth(),traceUI->getTargetLeafSize());
		//for(int i=0;i<scene->objects.size();i++){
		//	if(dynamic_cast<Trimesh*>(scene->objects[i])){
		//		Trimesh* triptr = dynamic_cast<Trimesh*>(scene->objects[i]);
		//		if(triptr->normals.size()==triptr->vertices.size()) triptr->itpl_normal = true;
		//		if(triptr->materials.size()==triptr->vertices.size()) triptr->itpl_material = true;
		//	}	
		//}
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if( !sceneLoaded() ) return false;

	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	if (buffer_width != w || buffer_height != h)
	{
		buffer_width = w;
		buffer_height = h;
		bufferSize = buffer_width * buffer_height * 3;
		delete[] buffer;
		delete[] aabuffer;
		buffer = new unsigned char[bufferSize];
		aabuffer = new unsigned char[bufferSize];
	}
	memset(buffer, 0, w*h*3);
	memset(aabuffer, 0, w*h*3);
	m_bBufferReady = true;
}

