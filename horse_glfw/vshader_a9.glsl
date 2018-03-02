#version 330 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 color;

uniform mat4 ModelView;
uniform mat4 Projection;



uniform mat4 Shader_Model;
uniform mat4 Shader_View;
uniform mat4 Shader_Projection;

void main()
{
    vec3 pos = (ModelView * vPosition).xyz;
	

    gl_Position = ModelView * vPosition * Projection;

//	gl_Position =  Shader_Projection * Shader_View * Shader_Model * vec4(vertex_horse, 1.0f);

    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
