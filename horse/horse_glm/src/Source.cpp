#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>	    // include GL Extension Wrangler
#include <GLFW/glfw3.h>     // include GLFW helper library

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assert.h>
#include "MatrixStack.h"
#include "Node.h"
#include "Horse.h"

using namespace glm;

typedef glm::vec4 point4;
typedef glm::vec4 color4;

// 36 vertices for a cube: (6 faces)(2 triangles/face)(3 vertices/triangle)
const int NumVertices = 36;

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {
	// Create the shaders
	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	} else {
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
	} else {
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
	if (!success) {
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
	if (!success) {
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

point4 points[NumVertices];
vec3 normals[NumVertices];

// eight points of a base cube
point4 vertices[8] =
{
    point4(-0.5, -0.5, 0.5, 1.0),
    point4(-0.5, 0.5, 0.5, 1.0),
    point4(0.5, 0.5, 0.5, 1.0),
    point4(0.5, -0.5, 0.5, 1.0),
    point4(-0.5, -0.5, -0.5, 1.0),
    point4(-0.5, 0.5, -0.5, 1.0),
    point4(0.5, 0.5, -0.5, 1.0),
    point4(0.5, -0.5, -0.5, 1.0)
};

//----------------------------------------------------------------------------

// Set up menu item indices, which we can also use with the joint angles
enum
{
    Torso = 0,
    Head = 1,
    LeftUpperArm = 2,
    LeftLowerArm = 3,
    RightUpperArm = 4,
    RightLowerArm = 5,
    LeftUpperLeg = 6,
    LeftLowerLeg = 7,
    RightUpperLeg = 8,
    RightLowerLeg = 9,
    Neck = 10,
    NumNodes,
    Quit
};

MatrixStack  mvstack;
mat4         base_model;
GLuint       shader_model;

// Joint angles with initial values
GLfloat theta[NumNodes] =
{
    -10.0,	// Torso
    -80.0,	// Head
    180.0,	// LeftUpperArm
    0.0,	// LeftLowerArm
    180.0,	// RightUpperArm
    0.0,	// RightLowerArm
    170.0,	// LeftUpperLeg
    10.0,	// LeftLowerLeg
    170.0,	// RightUpperLeg
    10.0,	// RightLowerLeg
    -45.0	// Neck
};

GLfloat target[NumNodes] =
{
    0.0,	// Torso
    0.0,	// Head
    0.0,	// LeftUpperArm
    0.0,	// LeftLowerArm
    0.0,	// RightUpperArm
    0.0,	// RightLowerArm
    0.0,	// LeftUpperLeg
    0.0,	// LeftLowerLeg
    0.0,	// RightUpperLeg
    0.0,	// RightLowerLeg
    0.0	    // Neck
};

GLint angle = Torso;

//----------------------------------------------------------------------------

Node nodes[NumNodes];

//----------------------------------------------------------------------------

int tmp_index = 0;
// given a face, initialize four points (two triangles) of it and their normals.
void quad(int a, int b, int c, int d)
{
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];

    vec3 normal = normalize(cross(vec3(u), vec3(v)));
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[a];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[b];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[c];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[a];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[c];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[d];
    tmp_index++;
}

// initialize the base cube and all the normals
void initBaseCube(void)
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

void traverse(Node* node)
{
    if (node == NULL)
    {
        return;
    }

    mvstack.push(base_model);

    base_model *= node->transform;
    node->render();

    if (node->child)
    {
        traverse(node->child);
    }

    base_model = mvstack.pop();

    if (node->sibling)
    {
        traverse(node->sibling);
    }
}

//----------------------------------------------------------------------------

void torso()
{
    mvstack.push(base_model);
    glm::mat4 translate = glm::translate(glm::mat4(0.1f), vec3(0.0f, 0.5f * TORSO_HEIGHT, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(0.1f), vec3(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH));
    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * translate * scale));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void neck()
{
    mvstack.push(base_model);
    glm::mat4 translate = glm::translate(glm::mat4(0.1f), vec3(0.0, 0.5 * NECK_HEIGHT, 0.0));
    glm::mat4 scale = glm::scale(glm::mat4(0.1f), vec3(NECK_WIDTH, NECK_HEIGHT, NECK_DEPTH));
    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * translate * scale));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void head()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * HEAD_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(HEAD_WIDTH, HEAD_HEIGHT, HEAD_DEPTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_upper_arm()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_lower_arm()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_upper_arm()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_lower_arm()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_upper_leg()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH)));
    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_lower_leg()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_upper_leg()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_lower_leg()
{
    mvstack.push(base_model);

    mat4 instance = (translate(mat4(1.0), vec3(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0)) * scale(mat4(1.0), vec3(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH)));

    glUniformMatrix4fv(shader_model, 1, GL_FALSE, value_ptr(base_model * instance));
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}


//----------------------------------------------------------------------------
mat4 RotateX(float f){
return rotate(glm::mat4(1.0f), glm::radians(f), glm::vec3(1, 0, 0));
}

mat4 RotateY(float f){
return rotate(glm::mat4(1.0f), glm::radians(f), glm::vec3(0, 1, 0));
}

mat4 RotateZ(float f){
return rotate(glm::mat4(1.0f), glm::radians(f), glm::vec3(0, 0, 1));
}

void initNodes(void)
{
    mat4  m;
    m = RotateY(theta[Torso]);
    nodes[Torso] = Node(m, torso, NULL, &nodes[Neck]);

    m = translate(mat4(1.0), vec3(TORSO_WIDTH / 2 - NECK_WIDTH / 2, TORSO_HEIGHT, 0.0)) * RotateZ(theta[Neck]);
    nodes[Neck] = Node(m, neck, &nodes[LeftUpperArm], &nodes[Head]);

    m = translate(mat4(1.0), vec3(0.0, NECK_HEIGHT, 0.0)) * RotateZ(theta[Head]);
    nodes[Head] = Node(m, head, NULL, NULL);

    m = translate(mat4(1.0), vec3(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_LEG_HEIGHT, -1.5 + UPPER_LEG_WIDTH / 2)) * RotateZ(theta[LeftUpperArm]);
    nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

    m = translate(mat4(1.0), vec3(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_ARM_HEIGHT, 1.5 - UPPER_ARM_WIDTH / 2))* RotateZ(theta[RightUpperArm]);
    nodes[RightUpperArm] = Node(m, right_upper_arm, &nodes[LeftUpperLeg], &nodes[RightLowerArm]);

    m = translate(mat4(1.0), vec3(-(TORSO_WIDTH / 2 - UPPER_ARM_WIDTH / 2), 0.1*UPPER_ARM_HEIGHT, -1.5 + UPPER_ARM_WIDTH / 2)) * RotateZ(theta[LeftUpperLeg]);
  nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

    m = translate(mat4(1.0), vec3(-(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2), 0.1*UPPER_LEG_HEIGHT, 1.5 - UPPER_LEG_WIDTH / 2)) * RotateZ(theta[RightUpperLeg]);
    nodes[RightUpperLeg] = Node(m, right_upper_leg, NULL, &nodes[RightLowerLeg]);

    m = translate(mat4(1.0), vec3(0.0, UPPER_ARM_HEIGHT, 0.0)) * RotateZ(theta[LeftLowerArm]);
    nodes[LeftLowerArm] = Node(m, left_lower_arm, NULL, NULL);

    m = translate(mat4(1.0), vec3(0.0, UPPER_ARM_HEIGHT, 0.0)) * RotateZ(theta[RightLowerArm]);
    nodes[RightLowerArm] = Node(m, right_lower_arm, NULL, NULL);

    m = translate(mat4(1.0), vec3(0.0, UPPER_LEG_HEIGHT, 0.0)) * RotateZ(theta[LeftLowerLeg]);
    nodes[LeftLowerLeg] = Node(m, left_lower_leg, NULL, NULL);

    m = translate(mat4(1.0), vec3(0.0, UPPER_LEG_HEIGHT, 0.0)) * RotateZ(theta[RightLowerLeg]);
    nodes[RightLowerLeg] = Node(m, right_lower_leg, NULL, NULL);
}


GLFWwindow* window;
const std::string TITLE = "RobotHorse";
const unsigned int WIDTH=800, HEIGHT=600;

int init_window(int width, int height, const std::string title)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (window == NULL)
    {
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
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    return 0;
}
int main()
{

    if (init_window(WIDTH, HEIGHT, TITLE) != 0)
    {
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    initBaseCube();

    // Initialize tree
    initNodes();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer[2];
    glGenBuffers(2, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);// vertices shader, layout=0
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);// vertices shader, layout=1
    glEnableVertexAttribArray(1);

    // Load shaders and use the resulting shader program
    GLuint program = loadShaders("vshader_a9.glsl", "fshader_a9.glsl");
    glUseProgram(program);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    shader_model = glGetUniformLocation(program, "Shader_Model");

    glm::vec3 c_pos = glm::vec3(0, 5, 20); // camera position
    glm::vec3 c_dir = glm::vec3(0.0f, 0.0f, 0.0f); // camera direction
    glm::vec3 c_up = glm::vec3(0, 1, 0); // tell the camera which way is 'up'
    glm::mat4 View = glm::lookAt(c_pos, c_dir, c_up);
    GLuint Shader_View = glGetUniformLocation(program, "Shader_View");
    glUniformMatrix4fv(Shader_View, 1, GL_FALSE, glm::value_ptr(View));

    float fov=45.0f;//perspective angle
    glm::mat4 Projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 300.0f);
    GLuint Shader_Projection = glGetUniformLocation(program, "Shader_Projection");
    glUniformMatrix4fv(Shader_Projection, 1, GL_FALSE, glm::value_ptr(Projection));

    base_model *= (&nodes[Torso])->transform;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        traverse(&nodes[Torso]);
       // base_model *= (&nodes[Torso])->transform;
        //(&nodes[Torso])->render();
       //(&nodes[Neck])->render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
