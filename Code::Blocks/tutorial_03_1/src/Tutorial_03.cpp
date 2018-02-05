// Include standard headers
#include "common/stdafx.h"

GLFWwindow* window;

int initWindow(int width, int height, const char* title);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
	if (initWindow(1024, 768, "Tutorial_03") != 0) {
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	/*
	generate vertex array object names
	1: Specifies the number of vertex array object names to generate
	&VertexArrayID: Specifies an array in which the generated vertex array object names are stored
	*/
	glGenVertexArrays(1, &VertexArrayID);
	/*
	binds the vertex array object with name "VertexArrayID". "VertexArrayID" is the name of a vertex array object previously returned from a call to glGenVertexArrays
	VertexArrayID: Specifies the name of the vertex array to bind
	*/
	glBindVertexArray(VertexArrayID);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	/*
	glGenBuffers returns "1" buffer object names in "vertexbuffer"
	No buffer objects are associated with the returned buffer object names until they are first bound by calling glBindBuffer.
	1: Specifies the number of buffer object names to be generated
	vertexbuffer: Specifies an array in which the generated buffer object names are stored
	*/
	glGenBuffers(1, &vertexbuffer);
	/*
	Specifies the target to which the buffer object is bound. The symbolic constant must be GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
	*/
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	/*
	creates and initializes a buffer object's data store
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// we do not change our shape in every loop, so we can put this piece of code here
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader (Specifies the index of the generic vertex attribute to be modified.)
		3,                  // size (pecifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer. The initial value is 4.)
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride (If stride is 0, the generic vertex attributes are understood to be tightly packed in the array. The initial value is 0.)
		(void*)0            // array buffer offset
		);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Rtrieve the location of the uniform variable within the specified program object, so we can set the value
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix: 45° Field of View, ration of "1024/768", dispaly range: 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1024.f / 768.f, 0.1f, 100.f);
	// Or, for an ortho camera:
	// glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),		// Camera is at (4, 3, 3) in World space
		glm::vec3(0, 0, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		//Head is up (set to 0, -1, 0 to look upside-down
		);

	// Model matrix: an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.f);

	// Our ModelViewProjection: multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;

	do {

		// Clear the screen
		/*
		sets the bitplane area of the window to values previously selected by glClearColor, glClearIndex, glClearDepth, glClearStencil, and glClearAccum.
		glClear takes a single argument that is the bitwise OR of several values indicating which buffer is to be cleared.
		The values are as follows:
		GL_COLOR_BUFFER_BIT		Indicates the buffers currently enabled for color writing.
		*/
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		// so every time we would render in the same way
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, in the "MVP" uniform

		glUniformMatrix4fv(
			MatrixID,		// For the vector and matrix commands, specifies a pointer to an array of count values that will be used to update the specified uniform variable.
			1,				// For the vector (glUniform*v) commands, specifies the number of elements that are to be modified. This should be 1 if the targeted uniform variable is not an array, and 1 or more if it is an array.
			GL_FALSE,		// For the matrix commands, specifies whether to transpose the matrix as the values are loaded into the uniform variable.
			&MVP[0][0]		// For the vector and matrix commands, specifies a pointer to an array of count values that will be used to update the specified uniform variable
			);

		/*
		By default, all client-side capabilities are disabled, including all generic vertex attribute arrays.
		If enabled, the values in the generic vertex attribute array will be accessed and used for rendering when calls are made to vertex array commands such as
		glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.
		*/
		glEnableVertexAttribArray(0);

		//glBindVertexArray(VertexArrayID);
		// Draw the triangle !
		/*
		0: Specifies the starting index in the enabled arrays.
		3: Specifies the number of indices to be rendered.
		*/
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		//glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

		glDisableVertexAttribArray(0);

		// swaps the front and back buffers of the specified window.
		glfwSwapBuffers(window);

		/*
		This function processes only those events that have already been received and then returns immediately. Processing events will cause the window and input callbacks associated with those events to be called.
		*/
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int initWindow(int width, int height, const char* title) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	// Needed for core profile
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		return -1;
	}

	return 0;
}
