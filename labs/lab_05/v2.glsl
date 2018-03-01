#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec4 col;
out vec3 nrm;
out vec3 fpo;

uniform mat4 m_m;
uniform mat4 v_m;
uniform mat4 p_m;

void main()
{
	gl_Position = p_m * v_m * m_m * vec4(pos, 1.0f);
	col = vec4(1,0,0, 1.0f);
	nrm = vec3(inverse(transpose(m_m)) * vec4(normal, 1.0f));
	fpo = vec3(m_m * vec4(pos, 1.0f));
}
