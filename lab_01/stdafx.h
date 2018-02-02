// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

// TODO: reference additional headers your program requires here
#include "..\external\glew\glew.h"	// include GL Extension Wrangler
#include "..\external\glfw\glfw3.h"	// include GLFW helper library
#include "..\external\glm\glm.hpp"
#include "..\external\glm\gtc\matrix_transform.hpp"
#include "..\external\glm\gtc\type_ptr.hpp"

int initWindow(GLFWwindow* window, int width, int height, std::string title);

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path);