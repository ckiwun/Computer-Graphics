#include "material.h"
#include "ray.h"
#include "light.h"
#include "../ui/TraceUI.h"
#include "../ui/GraphicalUI.h"
#include <assert.h>
extern TraceUI* traceUI;
class GraphicalUI;

#include "../fileio/bitmap.h"
#include "../fileio/pngimage.h"

using namespace std;
extern bool debugMode;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
Vec3d Material::shade(Scene *scene, const ray& r, const isect& i) const
{
  // For now, this method just returns the diffuse color of the object.
  // This gives a single matte color for every distinct surface in the
  // scene, and that's it.  Simple, but enough to get you started.
  // (It's also inconsistent with the phong model...)

  // Your mission is to fill in this method with the rest of the phong
  // shading model, including the contributions of all the light sources.

  // When you're iterating through the lights,
  // you'll want to use code that looks something
  // like this:
  //
  // for ( vector<Light*>::const_iterator litr = scene->beginLights(); 
  // 		litr != scene->endLights(); 
  // 		++litr )
  // {
  // 		Light* pLight = *litr;
  // 		.
  // 		.
  // 		.
  // }

  // You will need to call both the distanceAttenuation() and
  // shadowAttenuation() methods for each light source in order to
  // compute shadows and light falloff.

	Vec3d DPS(0,0,0);//Diffuse plus Specular Term;
	for (	vector<Light*>::const_iterator litr = scene->beginLights(); 
		litr != scene->endLights(); 
		++litr )
	{
		Light* pLight = *litr;
		ray shadowtest_ray(	r.at(i.t),
					pLight->getDirection(r.at(i.t)),
					ray::SHADOW);
		Vec3d Atten;
		if(traceUI->shadowSw())
			Atten = pLight->shadowAttenuation(shadowtest_ray,r.at(i.t))*pLight->distanceAttenuation(r.at(i.t));
		else
			Atten = Vec3d(1,1,1)*pLight->distanceAttenuation(r.at(i.t));
		Vec3d AttenLight = prod(Atten,pLight->getColor());
		Vec3d LightDir = pLight->getDirection(r.at(i.t));
		double DiffuseTerm = LightDir*i.N;
		Vec3d ReflLightDir = 2*DiffuseTerm*i.N - LightDir;
		if(DiffuseTerm<RAY_EPSILON) DiffuseTerm = 0;
		double SpecularTerm = (-r.getDirection())*ReflLightDir;
		if(SpecularTerm<RAY_EPSILON) SpecularTerm = 0;
		DPS += prod(AttenLight, (kd(i)*DiffuseTerm+ks(i)*pow(SpecularTerm,shininess(i))) );
	}
	Vec3d res = DPS+ke(i)+prod(ka(i),scene->ambient());
	res.clamp();
	return res;
}

TextureMap::TextureMap( string filename ) {

	int start = (int) filename.find_last_of('.');
	int end = (int) filename.size() - 1;
	if (start >= 0 && start < end) {
		string ext = filename.substr(start, end);
		if (!ext.compare(".png")) {
			png_cleanup(1);
			if (!png_init(filename.c_str(), width, height)) {
				double gamma = 2.2;
				int channels, rowBytes;
				unsigned char* indata = png_get_image(gamma, channels, rowBytes);
				int bufsize = rowBytes * height;
				data = new unsigned char[bufsize];
				for (int j = 0; j < height; j++)
					for (int i = 0; i < rowBytes; i += channels)
						for (int k = 0; k < channels; k++)
							*(data + k + i + j * rowBytes) = *(indata + k + i + (height - j - 1) * rowBytes);
				png_cleanup(1);
			}
		}
		else
			if (!ext.compare(".bmp")) data = readBMP(filename.c_str(), width, height);
			else data = NULL;
	} else data = NULL;
	if (data == NULL) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

Vec3d TextureMap::getMappedValue( const Vec2d& coord ) const
{
	Vec2d my_coord(coord);
	for(int i=0;i<2;i++){
		if(coord[i]<0) my_coord[i] = 0.0;
		if(coord[i]>1) my_coord[i] = 1.0;
	}
	double real_x = my_coord[0]*getWidth();
	double real_y = my_coord[1]*getHeight();
	int x = (int)real_x;
	int y = (int)real_y;
	int x_1 = x+1;
	int y_1 = y+1;

	double a0,a1,a2,a3;
	a0 = ((double)x_1-real_x)*((double)y_1-real_y);
	a1 = ((double)x_1-real_x)*(real_y-(double)y);
	a2 = (real_x-(double)x)*((double)y_1-real_y);
	a3 = (real_x-(double)x)*(real_y-(double)y);

	return	a0*getPixelAt(x_1,y_1)+a1*getPixelAt(x,y_1)+
		a2*getPixelAt(x_1,y)+a3*getPixelAt(x,y);
	
	// In order to add texture mapping support to the 
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.
	
	//return Vec3d(1,1,1);
	
}


Vec3d TextureMap::getPixelAt( int x, int y ) const
{
    // This keeps it from crashing if it can't load
    // the texture, but the person tries to render anyway.
    if (0 == data)
      return Vec3d(1.0, 1.0, 1.0);

    if( x >= width )
       x = width - 1;
    if( y >= height )
       y = height - 1;

    // Find the position in the big data array...
    int pos = (y * width + x) * 3;
    return Vec3d(double(data[pos]) / 255.0, 
       double(data[pos+1]) / 255.0,
       double(data[pos+2]) / 255.0);
}

Vec3d MaterialParameter::value( const isect& is ) const
{
    if( 0 != _textureMap )
        return _textureMap->getMappedValue( is.uvCoordinates );
    else
        return _value;
}

double MaterialParameter::intensityValue( const isect& is ) const
{
    if( 0 != _textureMap )
    {
        Vec3d value( _textureMap->getMappedValue( is.uvCoordinates ) );
        return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
    }
    else
        return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}

