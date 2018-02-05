//Lab 1
//modified from http://learnopengl.com/

#include "stdafx.h"
#include "ObjLoader.hpp"

using namespace std;

GLFWwindow* window;
glm::vec3 c_pos = glm::vec3(0, 0, 2); // camera position
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -2)); // camera direction
glm::vec3 c_up = glm::vec3(0, 1, 0); // tell the camera which way is 'up'
glm::mat4 view;

int initWindow();
void update_view();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {

	if (initWindow() != 0) {
		return -1;
	}
	
	glClearColor(.7, .7, .7, 0);

	GLuint shdr = loadShaders("vertex.shader", "fragment.shader");
	glUseProgram(shdr);

	vector<glm::vec3>v;
	vector<glm::vec3>nv;
	vector<glm::vec2>uv;

	loadOBJ("teddy.obj", v, nv, uv);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(glm::vec3), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


	// ---- MODEL MATRIX -----
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.65f, 0.65f, 0.65f));
	glm::mat4 translate = glm::translate(glm::mat4(0.1f), glm::vec3(0, -1, 0));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-45.f), glm::vec3(0, 0, 1));
	glm::mat4 translate2 = glm::translate(glm::mat4(0.1f), glm::vec3(0, 1, 0));

	model = translate2 * scale * rotate * translate;


	// ---- VIEW MATRIX -----
	view = glm::lookAt(c_pos, c_pos + c_dir, c_up);

	// ---- PROJECTION MATRIX -----
	glm::mat4 pm = glm::perspective(45.f, 800.f / 600.f, 0.1f, 100.f);


	//can be substitude with the functions defined in ShaderProg.h
	GLuint mm_addr = glGetUniformLocation(shdr, "model"); // Model matrix address (from vertex shader)
	GLuint vm_addr = glGetUniformLocation(shdr, "view"); // View matrix address
	GLuint pm_addr = glGetUniformLocation(shdr, "projection"); // Projection matrix address
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(model));
	glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(pm));
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


	//game loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// Swap the screen buffers
		glfwSwapBuffers(window);

		// Render
		// Clear the colorbuffer
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(VAO);

		glUniformMatrix4fv(vm_addr, 1, false, glm::value_ptr(view));
		glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, v.size());


		glBindVertexArray(0);


	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void update_view() {
	view = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

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

int initWindow() {

	glfwInit();

	window = glfwCreateWindow(800, 600, "First window", nullptr, nullptr);

	glfwSetKeyCallback(window, keyCallback);

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