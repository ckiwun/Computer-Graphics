#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/ray.h"
#include "scene/scene.h"
#include "scene/cubeMap.h"
#include "ui/TraceUI.h"
#include <time.h>
#include <queue>

class Scene;

class RayTracer
{
public:
	RayTracer();
        ~RayTracer();

	Vec3d tracePixel(int i, int j);
	Vec3d blocktracePixel(int i, int j);
	Vec3d trace(double x, double y);
	Vec3d traceRay(ray& r, int depth);
	Vec3d AntiAlias(int i,int j,bool needaa);

	void getBuffer(unsigned char *&buf, int &w, int &h);
	double aspectRatio();

	void traceSetup( int w, int h );

	bool loadScene(char* fn);
	bool sceneLoaded() { return scene != 0; }

	void setReady(bool ready) { m_bBufferReady = ready; }
	bool isReady() const { return m_bBufferReady; }

	const Scene& getScene() { return *scene; }
	bool haveCubeMap() { return cubemap!=nullptr; }
	CubeMap* getCubeMap() { return cubemap; }
	void setCubeMap(CubeMap* cm) { cubemap = cm; } 

public:
        unsigned char *buffer;
        int buffer_width, buffer_height;
        int bufferSize;
        unsigned char *aabuffer;
        Scene* scene;
	CubeMap* cubemap;

        bool m_bBufferReady;
};

#endif // __RAYTRACER_H__
