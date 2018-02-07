#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_grid;
layout(location = 1) in vec3 vertex_coordinate;

// Output data ; will be interpolated for each fragment.
out vec3 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertex_grid,1);

	fragmentColor = vertex_grid;
}
