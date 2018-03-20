//Lab 1
//modified from http://learnopengl.com/

#include "common/stdafx.h"

using namespace std;

GLFWwindow* window;
glm::vec3 c_pos = glm::vec3(0, 0, 2);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -2));
glm::vec3 c_up = glm::vec3(0, 1, 0);
glm::mat4 vm;

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

	std::vector<glm::vec3> v = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 1, 0)
	};

	std::vector<glm::vec3> ps = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0)
	};

    std::vector<glm::vec3> color_diff = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0)
	};

    std::vector<unsigned int> i={0,1,2, 2, 0, 1};
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v.size(), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);// only read three vertices

	GLuint VBOps;
	glGenBuffers(1, &VBOps);
    glBindBuffer(GL_ARRAY_BUFFER, VBOps);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*ps.size(), &ps[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint VBOcolor;
	glGenBuffers(1, &VBOcolor);
    glBindBuffer(GL_ARRAY_BUFFER, VBOcolor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*color_diff.size(), &color_diff[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glVertexAttribDivisor(0, 0);// one vertex to one vertex
	glVertexAttribDivisor(1, 1);// one vertex to one intance
	glVertexAttribDivisor(2, 1);


	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*i.size(), &i[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

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
	GLuint pm_addr = glGetUniformLocation(shdr, "p_m");
	GLuint vm_addr = glGetUniformLocation(shdr, "v_m");
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
	glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(pm));


	while (!glfwWindowShouldClose(window)) {

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
		//glDrawArrays(GL_TRIANGLES, 0, v.size());

		//glDrawArraysInstanced(GL_TRIANGLES, 0, v.size(), 3);

        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glDrawElementsInstanced(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0, 3);

		translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, -0.5, 0));
		rotate = glm::rotate(glm::mat4(1.0f), glm::radians(45.f), glm::vec3(0, 0, 1));
		mm = mem * translate * _anchor * rotate * anchor * scale;
		glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
		//glDrawArrays(GL_TRIANGLES, 0, v.size());



		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	return 0;
}

void update_view() {
	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
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

	window = glfwCreateWindow(800, 600, "First windwo", nullptr, nullptr);

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
