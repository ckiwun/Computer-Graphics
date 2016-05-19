
//Chi-Wen Cheng, cc59884
uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D decal;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 objectToWorld;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 halfAngle;
varying vec3 c0, c1, c2;

void main()
{
	vec2 my_texcoord = normalMapTexCoord;
	float temp = my_texcoord.x;
	my_texcoord.x = my_texcoord.y;
	my_texcoord.y = temp;
	my_texcoord.x = -6.0 * my_texcoord.x;
	my_texcoord.y = 2.0 * my_texcoord.y;

	vec3 normal = texture2D(normalMap,my_texcoord).xyz;
	normal = normalize(2.0*normal - vec3(1.0,1.0,1.0));
	float diffuse_term = max(0.0,dot(normal,normalize(lightDirection)));
	gl_FragColor = LMa + LMd * diffuse_term;
}
