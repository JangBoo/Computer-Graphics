#version 330 core

uniform vec3 l_p;
uniform vec3 viewPos;

out vec4 color;

in vec4 col;
in vec3 nrm;
in vec3 fpo;

void main()
{
	vec3 light_color = vec3(1, 1, 1);
	vec3 ambient = 0.3 * light_color;

	vec3 ld = normalize(l_p - fpo);
	vec3 n = normalize(nrm);
	float angle = dot(n, ld);
	float diff = max(0.0, angle);
	vec3 diffuse = diff * light_color;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fpo);
	vec3 reflectDir = normalize(reflect(-ld, n));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength* spec*light_color;
	
	vec4 result = vec4(diffuse+ambient+specular, 1);
 	color = result * col;
}
