//Lab 1
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include "..\glm.hpp"
#include "gtc\matrix_transform.hpp"
#include "gtc\type_ptr.hpp"

#include <vector>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

GLFWwindow* window;
glm::vec3 c_pos = glm::vec3(0, 0, 2);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -2));
glm::vec3 c_up = glm::vec3(0, 1, 0);
glm::mat4 vm;

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path);
void update_view();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


int init() {

	glfwInit();

	window = glfwCreateWindow(800, 600, "First windwo", nullptr, nullptr);

	glfwSetKeyCallback(window, key_callback);

	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != 0) {
		return -1;
	}

	return 0;
}

int main() {

	if (init() != 0) {
		return -1;
	}

	GLuint shdr = loadShaders("vertex.shader", "fragment.shader");
	glUseProgram(shdr);

	std::vector<glm::vec3> v = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0)
	};


	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v.size(), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//------ MODEL MATRIX ---------
	glm::mat4 mm;
	glm::mat4 scale;
	glm::mat4 translate;
	glm::mat4 rotate;

	//------ VIEW MATRIX ---------

	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);

	//------ PROJECTION MATRIX -------
	glm::mat4 pm = glm::perspective(45.f, 800.f / 600.f, 0.1f, 100.f);

	GLuint mm_addr = glGetUniformLocation(shdr, "m_m");
	GLuint vm_addr = glGetUniformLocation(shdr, "v_m");
	GLuint pm_addr = glGetUniformLocation(shdr, "p_m");
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
	glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(pm));

	glClearColor(.7, .7, .7, 0);
	while (!glfwWindowShouldClose(window)) {


		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniformMatrix4fv(vm_addr, 1, false, glm::value_ptr(vm));

		glm::mat4 anchor = glm::translate(glm::mat4(1.0f), -glm::vec3(0, 0.5, 0));
		glm::mat4 _anchor = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0));

		scale = glm::scale(glm::mat4(1.0f), glm::vec3(.5, .5, .5));
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

		glm::mat4 mem = translate * _anchor * rotate * anchor;
		mm = mem * scale;

		glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
		glDrawArrays(GL_TRIANGLES, 0, v.size());

		scale = glm::scale(glm::mat4(1.0f), glm::vec3(.5, .5, .5));
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, -0.5, 0));

		rotate = glm::rotate(glm::mat4(1.0f), glm::radians(45.f), glm::vec3(0, 0, 1));


		mm = mem * translate * _anchor * rotate * anchor * scale;


		glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
		glDrawArrays(GL_TRIANGLES, 0, v.size());


	}
	return 0;
}

void update_view() {
	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_DOWN) {
		c_pos.z += 1;
		update_view();
	}

	if (key == GLFW_KEY_UP) {
		c_pos.z -= 1;
		update_view();
	}

}

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {
	// Create the shaders
	// Read the Vertex Shader code from the file
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	return shaderProgram;
}