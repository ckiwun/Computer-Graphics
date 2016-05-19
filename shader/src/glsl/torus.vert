//Chi-Wen Cheng, cc59884
#define M_PI 3.1415926535897932384626433832795

attribute vec2 parametric; // theta,phi

uniform vec3 lightPosition;  // Object-space
uniform vec3 eyePosition;    // Object-space
uniform vec2 torusInfo; //outer-radius,inner-radius

varying vec2 normalMapTexCoord;

varying vec3 lightDirection;
varying vec3 halfAngle;
varying vec3 eyeDirection;
varying vec3 c0, c1, c2;

void main()
{
	vec3 torus_coord;
	vec2 my_param = 2.0 * M_PI * parametric;
	torus_coord.x = (torusInfo.x+torusInfo.y*cos(my_param.x))*cos(my_param.y);
	torus_coord.y = (torusInfo.x+torusInfo.y*cos(my_param.x))*sin(my_param.y);
	torus_coord.z = torusInfo.y*sin(my_param.x);
	normalMapTexCoord = parametric;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(torus_coord, 1);
	vec3 grad_u,grad_v;
	grad_u.x = -2.0*M_PI*torusInfo.y*sin(my_param.x)*cos(my_param.y);
	grad_u.y = -2.0*M_PI*torusInfo.y*sin(my_param.x)*sin(my_param.y);
	grad_u.z = 2.0*M_PI*torusInfo.y*cos(my_param.x);
	grad_v.x = -2.0*M_PI*(torusInfo.x+torusInfo.y*cos(my_param.x))*sin(my_param.y); 
	grad_v.y = 2.0*M_PI*(torusInfo.x+torusInfo.y*cos(my_param.x))*cos(my_param.y); 
	grad_v.z = 0.0; 
	vec3 tangent, normal, binormal;
	tangent = normalize(grad_u);
	normal = cross(normalize(grad_v),normalize(grad_u));
	binormal = cross(tangent,normal);
	mat3 M = mat3(tangent,binormal,normal);
	eyeDirection = eyePosition - torus_coord;
	eyeDirection = eyeDirection * M;
	lightDirection = lightPosition - torus_coord;
	lightDirection = lightDirection * M;
	halfAngle = (eyeDirection+lightDirection)/2.0;
	c0 = tangent;
	c1 = binormal;
	c2 = normal;
}

