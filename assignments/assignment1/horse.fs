#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Output data
out vec3 color;

uniform vec3 horse_color;

uniform vec4 color_x;
uniform vec4 color_y;
uniform vec4 color_z;

void main(){
	color = horse_color;
}
