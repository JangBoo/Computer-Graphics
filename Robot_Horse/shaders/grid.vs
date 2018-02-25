#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_grid;

// Output data ; will be interpolated for each fragment.
out vec4 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertex_grid, 1.0f);

	fragmentColor = vec4(vertex_grid, 1.0f);
}
