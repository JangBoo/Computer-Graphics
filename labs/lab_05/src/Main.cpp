#include "common/stdafx.h"

#include "objloader.hpp"

#define STP		0.5f

GLFWwindow* window;
glm::vec3 c_pos = glm::vec3(0, 0, 2);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -2));
glm::vec3 c_up = glm::vec3(0, 1, 0);
glm::mat4 vm;
float ry = 0.0f;
float rx = 0.0f;

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";
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
		printf("Impossible to open %s. Are you in the right directory ?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glBindAttribLocation(ProgramID, 0, "in_Position");

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID); //free up memory
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void update_view() {
	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_A) {
		c_pos.z += 1;
		update_view();
	}

	if (key == GLFW_KEY_W) {
		c_pos.z -= 1;
		update_view();
	}

	if (key == GLFW_KEY_RIGHT) ry -= STP;
	if (key == GLFW_KEY_LEFT) ry += STP;
	if (key == GLFW_KEY_UP) rx += STP;
	if (key == GLFW_KEY_DOWN) rx -= STP;

}

int init() {

	glfwInit();

	window = glfwCreateWindow(800, 600, "First window", nullptr, nullptr);

	glfwSetKeyCallback(window, key_callback);

	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != 0) {
		return -1;
	}

    glEnable(GL_DEPTH_TEST);
	return 0;
}

int main() {

	if (init() != 0) {
		return -1;
	}

	GLuint shdr = loadShaders("v.glsl", "f.glsl");
	glUseProgram(shdr);


    std::vector<glm::vec3> v;
	std::vector<glm::vec3> nv;
	std::vector<glm::vec2> uv;

	loadOBJ("t.obj", v, nv, uv);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    GLuint VBO_pos;
	glGenBuffers(1, &VBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v.size(), &v[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

    GLuint VBO_norm;
	glGenBuffers(1, &VBO_norm);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v.size(), &nv[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//------ MODEL MATRIX ---------
	glm::mat4 mm;
	glm::mat4 scale;
	glm::mat4 translate;
	glm::mat4 rotate;

	//------ VIEW MATRIX ---------

	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);

	//------ PROJECTION MATRIX -------
	glm::mat4 pm = glm::perspective(45.f, 800.f / 600.f, 0.1f, 100.f);


    //------ LIGHT -------
	glm::vec3 light_position = glm::vec3(1, 3, 0);

	//------ SHADER UNIFORMS -----
	GLuint mm_addr = glGetUniformLocation(shdr, "m_m");
	GLuint vm_addr = glGetUniformLocation(shdr, "v_m");
	GLuint pm_addr = glGetUniformLocation(shdr, "p_m");
	glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(pm));

    //------ LIGHT -------
    GLuint lp_addr = glGetUniformLocation(shdr, "l_p");
    GLuint viewPos_addr = glGetUniformLocation(shdr, "viewPos");
	glUniform3fv(lp_addr, 1, glm::value_ptr(light_position));
	glUniform3fv(viewPos_addr, 1, glm::value_ptr(c_pos));


	glClearColor(.7, .7, .7, 0);
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniformMatrix4fv(vm_addr, 1, false, glm::value_ptr(vm));

		scale = glm::scale(glm::mat4(1.0f), glm::vec3(.5, .5, .5));
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		rotate = glm::rotate(glm::mat4(1.0f), glm::radians(ry), glm::vec3(0, 1, 0));
		rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(rx), glm::vec3(1, 0, 0));

		mm = translate * rotate * scale;

		glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
		glDrawArrays(GL_TRIANGLES, 0, v.size());
	}
	return 0;
}
