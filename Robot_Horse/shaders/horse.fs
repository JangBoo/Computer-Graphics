#version 330 core

// Interpolated values from the vertex shaders
in vec4 fragmentColor;

// Output data
out vec4 color;

uniform vec4 horse_color;

void main(){
	color = horse_color;
}
