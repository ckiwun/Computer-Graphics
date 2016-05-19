#include <cmath>

#include "light.h"

using namespace std;

double DirectionalLight::distanceAttenuation(const Vec3d& P) const
{
  // distance to light is infinite, so f(di) goes to 0.  Return 1.
  return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation(const ray& r, const Vec3d& p) const
{
  // You should implement shadow-handling code here.
  isect i;
  ray test_ray(r.p,r.d,r.t);
  if(getScene()->intersect(test_ray,i))
	return i.material->kd(i)*i.material->kt(i);
  return Vec3d(1,1,1);
}

Vec3d DirectionalLight::getColor() const
{
  return color;
}

Vec3d DirectionalLight::getDirection(const Vec3d& P) const
{
  // for directional light, direction doesn't depend on P
  return -orientation;
}

double PointLight::distanceAttenuation(const Vec3d& P) const
{
  // You'll need to modify this method to attenuate the intensity 
  // of the light based on the distance between the source and the 
  // point P.  For now, we assume no attenuation and just return 1.0
  double distance = (P-position).length();
  double dominator = constantTerm+distance*linearTerm+distance*distance*quadraticTerm;
  return min(1.0,1/dominator);
}

Vec3d PointLight::getColor() const
{
  return color;
}

Vec3d PointLight::getDirection(const Vec3d& P) const
{
  Vec3d ret = position - P;
  ret.normalize();
  return ret;
}


Vec3d PointLight::shadowAttenuation(const ray& r, const Vec3d& p) const
{
  // You should implement shadow-handling code here.
  isect i;
  ray test_ray(r.p,r.d,r.t);
  getScene()->intersect(test_ray,i);
  Vec3d E1 = position-r.at(i.t);
  Vec3d E2 = p-r.at(i.t);
  if(E1*E2<0) //some barrier between path
	return Vec3d(0,0,0);
  return Vec3d(1,1,1);
}
