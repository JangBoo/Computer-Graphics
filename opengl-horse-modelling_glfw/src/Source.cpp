#include <GL/glew.h>	    // include GL Extension Wrangler
#include <GLFW/glfw3.h>     // include GLFW helper library

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Angel.h"
#include <assert.h>
#include "MatrixStack.h"
#include "Node.h"
#include "Horse.h"

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36;	//(6 faces)(2 triangles/face)(3 vertices/triangle)


point4 points[NumVertices];
vec3   normals[NumVertices];

// base cube
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
mat4         model_view;
GLuint       ModelView, Projection;

int Index = 0;

// Joint angles with initial values
GLfloat
theta[NumNodes] =
{
    0.0,	// Torso
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

GLfloat
target[NumNodes] =
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

Node  nodes[NumNodes];

//----------------------------------------------------------------------------

// color the cube
void
quad(int a, int b, int c, int d)
{
    // Initialize temporary vectors along the quad's edge to
    // Compute its face normal
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];

    vec3 normal = normalize(cross(u, v));
    normals[Index] = normal;
    points[Index] = vertices[a];
    Index++;
    normals[Index] = normal;
    points[Index] = vertices[b];
    Index++;
    normals[Index] = normal;
    points[Index] = vertices[c];
    Index++;
    normals[Index] = normal;
    points[Index] = vertices[a];
    Index++;
    normals[Index] = normal;
    points[Index] = vertices[c];
    Index++;
    normals[Index] = normal;
    points[Index] = vertices[d];
    Index++;
}

void
colorcube(void)
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

void
traverse(Node* node)
{
    if (node == NULL)
    {
        return;
    }

    mvstack.push(model_view);

    model_view *= node->transform;
    node->render();

    if (node->child)
    {
        traverse(node->child);
    }

    model_view = mvstack.pop();

    if (node->sibling)
    {
        traverse(node->sibling);
    }
}

//----------------------------------------------------------------------------

void
torso()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) * Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
neck()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * NECK_HEIGHT, 0.0) * Scale(NECK_WIDTH, NECK_HEIGHT, NECK_DEPTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
head()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) * Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_DEPTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_upper_arm()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_lower_arm()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_upper_arm()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_lower_arm()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_upper_leg()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
left_lower_leg()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_upper_leg()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}

void
right_lower_leg()
{
    mvstack.push(model_view);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    model_view = mvstack.pop();
}


//----------------------------------------------------------------------------

void
initNodes(void)
{
    mat4  m;

    m = RotateY(theta[Torso]);
    nodes[Torso] = Node(m, torso, NULL, &nodes[Neck]);

    m = Translate(TORSO_WIDTH / 2 - NECK_WIDTH / 2, TORSO_HEIGHT, 0.0) * RotateZ(theta[Neck]);
    nodes[Neck] = Node(m, neck, &nodes[LeftUpperArm], &nodes[Head]);

    m = Translate(0.0, NECK_HEIGHT, 0.0) * RotateZ(theta[Head]);
    nodes[Head] = Node(m, head, NULL, NULL);

    m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_LEG_HEIGHT, -1.5 + UPPER_LEG_WIDTH / 2) * RotateZ(theta[LeftUpperArm]);
    nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

    m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_ARM_HEIGHT, 1.5 - UPPER_ARM_WIDTH / 2) * RotateZ(theta[RightUpperArm]);
    nodes[RightUpperArm] = Node(m, right_upper_arm, &nodes[LeftUpperLeg], &nodes[RightLowerArm]);

    m = Translate(-(TORSO_WIDTH / 2 - UPPER_ARM_WIDTH / 2), 0.1*UPPER_ARM_HEIGHT, -1.5 + UPPER_ARM_WIDTH / 2) * RotateZ(theta[LeftUpperLeg]);
    nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

    m = Translate(-(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2), 0.1*UPPER_LEG_HEIGHT, 1.5 - UPPER_LEG_WIDTH / 2) * RotateZ(theta[RightUpperLeg]);
    nodes[RightUpperLeg] = Node(m, right_upper_leg, NULL, &nodes[RightLowerLeg]);

    m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateZ(theta[LeftLowerArm]);
    nodes[LeftLowerArm] = Node(m, left_lower_arm, NULL, NULL);

    m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateZ(theta[RightLowerArm]);
    nodes[RightLowerArm] = Node(m, right_lower_arm, NULL, NULL);

    m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateZ(theta[LeftLowerLeg]);
    nodes[LeftLowerLeg] = Node(m, left_lower_leg, NULL, NULL);

    m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateZ(theta[RightLowerLeg]);
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
    colorcube();

    // Initialize tree
    initNodes();

    theta[Torso] = 50.0;
    nodes[Torso].transform = RotateY(theta[Torso]);



    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points)+sizeof(normals), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader_a9.glsl", "fshader_a9.glsl");
    glUseProgram(program);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));


    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
/*

glm::vec3 c_pos = glm::vec3(0, 0, 200); // camera position
glm::vec3 c_dir = glm::vec3(0.0f, 0.0f, 0.0f); // camera direction
glm::vec3 c_up = glm::vec3(0, 1, 0); // tell the camera which way is 'up'
glm::mat4 View = glm::lookAt(c_pos, c_dir, c_up);
float fov=45.0f;//perspective angle
glm::mat4 projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 300.0f);

   GLuint MVP = glGetUniformLocation(program, "Shader_View");
   glUniformMatrix4fv(MVP, 1, GL_TRUE, &View[0][0]);
*/


    while (!glfwWindowShouldClose(window))
    {
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glUniformMatrix4fv(Projection, 1, GL_TRUE, &projection[0][0]);


	GLfloat left = -10.0, right = 10.0;
	GLfloat bottom = -10.0, top = 10.0;
	GLfloat zNear = -10.0, zFar = 10.0;

	GLfloat aspect = GLfloat(WIDTH) / HEIGHT;

	if (aspect > 1.0)
	{
		left *= aspect;
		right *= aspect;
	}
	else
	{
		bottom /= aspect;
		top /= aspect;
	}

	mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);



        traverse(&nodes[Torso]);


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
