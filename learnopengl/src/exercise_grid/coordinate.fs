#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Output data
out vec3 color;

uniform vec4 color_x;
uniform vec4 color_y;
uniform vec4 color_z;

void main(){
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	color = vec3(1.0f, 0.0f, 0.0f);
}
