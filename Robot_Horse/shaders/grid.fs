#version 330 core

// Interpolated values from the vertex shaders
in vec4 fragmentColor;

// Output data
out vec4 color;

void main(){
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
