#include <iostream>

#include <GL/glew.h>	    // include GL Extension Wrangler
#include <GLFW/glfw3.h>     // include GLFW helper library

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_gl.h>

#include <Angel.h>
#include <assert.h>

#include "Vertices.h"

#include "MatrixStack.h"
#include "Node.h"
#include "Horse.h"

const std::string TITLE = "RobotHorse";
const unsigned int WIDTH=1920, HEIGHT=1080;

const std::string grid_vs = "shaders/grid.vs";
const std::string grid_fs = "shaders/grid.fs";

const std::string axis_vs = "shaders/axis.vs";
const std::string axis_fs = "shaders/axis.fs";

const std::string horse_vs = "vshader_a9.glsl";
const std::string horse_fs = "fshader_a9.glsl";

const int gridX = 50;
const int gridZ = 50;

GLFWwindow* window;

glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;

// ---- VIEW MATRIX global variables -----
float c_rotate_xz = 0.0f;
float c_rotate_y = 10.0f;
glm::vec3 c_pos = glm::vec3(sin(c_rotate_xz) * 20, c_rotate_y, cos(c_rotate_xz) * 20); // camera position
glm::vec3 c_dir = glm::vec3(0.0f, 0.0f, 0.0f); // camera direction
glm::vec3 c_up = glm::vec3(0, 1, 0); // tell the camera which way is 'up'

float moveOnX=0,moveOnZ=0;
const float RANGE_MIN=-45.0f,RANGE_MAX=45.0f;
float scaler=1.0f;
float rotater=0.0f;
glm::vec3 rotateOrientation=glm::vec3(0.0f,1.0f,0.0f);

//camera attributes
float yaw=-90.0f;
float pitch=0.0f;
float lastX=WIDTH/2.0f;
float lastY=HEIGHT/2.0f;
float fov=45.0f;//perspective angle
bool leftMouseButton=false, middleMouseButton=false, rightMouseButton=false;//varibles for detecting the mouse button action
bool firstMouse=true;

int init_window(int width, int height, const std::string title);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// 36 vertices for a cube: (6 faces)(2 triangles/face)(3 vertices/triangle)
const int NumVertices = 36;

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

    vec3 normal = normalize(cross(u, v));
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

    mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) * Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void neck()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * NECK_HEIGHT, 0.0) * Scale(NECK_WIDTH, NECK_HEIGHT, NECK_DEPTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void head()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) * Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_DEPTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_upper_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_lower_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_upper_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_lower_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_upper_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.65f, 0.65f, 0.65f));
    glm::mat4 translate = glm::translate(glm::mat4(0.1f), glm::vec3(0, -1, 0));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_lower_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_upper_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_lower_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}


//----------------------------------------------------------------------------

void initNodes(void)
{
    mat4  m;

    m = Translate(0.0, 1.9*TORSO_HEIGHT, 0.0) * RotateY(theta[Torso]);
    nodes[Torso] = Node(m, torso, NULL, &nodes[Neck]);

    m = Translate(TORSO_WIDTH / 2 - NECK_WIDTH / 2, TORSO_HEIGHT, 0.0) * RotateZ(theta[Neck]);
    nodes[Neck] = Node(m, neck, &nodes[LeftUpperArm], &nodes[Head]);

    m = Translate(0.0, NECK_HEIGHT, 0.0) * RotateZ(theta[Head]);
    nodes[Head] = Node(m, head, NULL, NULL);

    m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_LEG_HEIGHT, -TORSO_DEPTH/2 + UPPER_LEG_WIDTH / 2) * RotateZ(theta[LeftUpperArm]);
    nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

    m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_ARM_HEIGHT, TORSO_DEPTH/2 - UPPER_ARM_WIDTH / 2) * RotateZ(theta[RightUpperArm]);
    nodes[RightUpperArm] = Node(m, right_upper_arm, &nodes[LeftUpperLeg], &nodes[RightLowerArm]);

    m = Translate(-(TORSO_WIDTH / 2 - UPPER_ARM_WIDTH / 2), 0.1*UPPER_ARM_HEIGHT, -TORSO_DEPTH/2 + UPPER_ARM_WIDTH / 2) * RotateZ(theta[LeftUpperLeg]);
    nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

    m = Translate(-(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2), 0.1*UPPER_LEG_HEIGHT, TORSO_DEPTH/2 - UPPER_LEG_WIDTH / 2) * RotateZ(theta[RightUpperLeg]);
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



int main()
{

    if (init_window(WIDTH, HEIGHT, TITLE) != 0)
    {
        return -1;
    }

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // build and compile our shader zprogram
    // ------------------------------------
    GLuint shader_grid = loadShaders(grid_vs, grid_fs);
    GLuint grid_mvp = glGetUniformLocation(shader_grid, "MVP");

    GLuint shader_axis = loadShaders(axis_vs, axis_fs);
    GLuint axis_mvp = glGetUniformLocation(shader_axis, "MVP");

    GLuint shader_horse = loadShaders(horse_vs, horse_fs);
    shader_model = glGetUniformLocation(shader_horse, "Shader_Model");
    GLuint Shader_View = glGetUniformLocation(shader_horse, "Shader_View");
    GLuint Shader_Projection = glGetUniformLocation(shader_horse, "Shader_Projection");

    GLuint vertexArray_grid, vertexBuffer_grid;
    glGenVertexArrays(1, &vertexArray_grid);
    glGenBuffers(1, &vertexBuffer_grid);
    glBindVertexArray(vertexArray_grid);
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray_grid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_grid), buffer_data_grid, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    GLuint vertexArray_axis, vertexBuffer_axis[2];
    glGenVertexArrays(1, &vertexArray_axis);
    glGenBuffers(2, vertexBuffer_axis);
    glBindVertexArray(vertexArray_axis);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_axis[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_axis), buffer_data_axis, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_axis[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_axis), buffer_data_axis, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


    initBaseCube();
    initNodes();

    //theta[Torso] = 50.0;
    //nodes[Torso].transform = RotateY(theta[Torso]);

    GLuint vertexArray_horse, vertexBuffer_horse[2];
    glGenVertexArrays(1, &vertexArray_horse);
    glGenBuffers(2, vertexBuffer_horse);
    glBindVertexArray(vertexArray_horse);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_horse[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);// vertices shader, layout=0
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_horse[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);// vertices shader, layout=1
    glEnableVertexAttribArray(1);

    // Model matrix: an identity matrix (model will be at the origin)
    Model = glm::mat4(1.0f);

    // Projection matrix: 45 Field of View, ration of "1024/768", dispaly range: 0.1 unit <-> 100 units
    Projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
    // Or, for an ortho camera:
    // glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates


    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
        By default, all client-side capabilities are disabled, including all generic vertex attribute arrays.
        If enabled, the values in the generic vertex attribute array will be accessed and used for rendering when calls are made to vertex array commands such as
        glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.
        */
        glEnableVertexAttribArray(0);
        glBindVertexArray(vertexArray_grid);
        glUseProgram(shader_grid);

        c_pos = glm::vec3(sin(c_rotate_xz) * 20, c_rotate_y, cos(c_rotate_xz) * 20); // camera position
        // Camera matrix
        View = glm::lookAt(c_pos, c_dir, c_up);
        // Our ModelViewProjection: multiplication of our 3 matrices
        glm::mat4 MVP = Projection * View * Model;

        //////////////////////////////////////////////////////////
        //                                                      //
        // Draw Cartesian coordinate system                     //
        //      Draw a square, then translate it                //
        //                                                      //
        //////////////////////////////////////////////////////////
        for(int i=1; i<=gridX; ++i)
        {
            glm::mat4 anchor_x1 = MVP * glm::translate(glm::mat4(1.0f), glm::vec3(i-1, 0.f, 0.f));
            for(int j=1; j<=gridZ; ++j)
            {
                glm::mat4 anchor_z1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, j-1));
                glm::mat4 mvp1 = anchor_x1 * anchor_z1;
                glUniformMatrix4fv(grid_mvp, 1, GL_FALSE, &mvp1[0][0]);
                glDrawArrays(GL_LINE_LOOP, 0, 4*2);

                glm::mat4 anchor_z2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -j));
                glm::mat4 mvp2 = anchor_x1 * anchor_z2;
                glUniformMatrix4fv(grid_mvp, 1, GL_FALSE, &mvp2[0][0]);
                glDrawArrays(GL_LINE_LOOP, 0, 4*2);
            }

            glm::mat4 anchor_x2 = MVP * glm::translate(glm::mat4(1.0f), glm::vec3(-i, 0.f, 0.f));
            for(int j=1; j<=gridZ; ++j)
            {
                glm::mat4 anchor_1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, j-1));
                glm::mat4 mvp1 = anchor_x2 * anchor_1;
                glUniformMatrix4fv(grid_mvp, 1, GL_FALSE, &mvp1[0][0]);
                glDrawArrays(GL_LINE_LOOP, 0, 4*2);

                glm::mat4 anchor_2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -j));
                glm::mat4 mvp2 = anchor_x2 * anchor_2;
                glUniformMatrix4fv(grid_mvp, 1, GL_FALSE, &mvp2[0][0]);
                glDrawArrays(GL_LINE_LOOP, 0, 12*3);
            }
        }

        //glEnableVertexAttribArray(0);
        //glEnableVertexAttribArray(1);

        //////////////////////////////////////////////////////////
        //                                                      //
        // Draw X, Y, Z axes                                    //
        //                                                      //
        //////////////////////////////////////////////////////////
        glBindVertexArray(vertexArray_axis);
        glUseProgram(shader_axis);
        glm::mat4 c_mvp = Projection * View * Model;
        glUniformMatrix4fv(axis_mvp, 1, GL_FALSE, glm::value_ptr(c_mvp));

        glEnable(GL_LINE_SMOOTH);

        glLineWidth(5.0f);
        glDrawArrays(GL_LINES, 0, 3*2);
        glDrawArrays(GL_LINES, 6, 3*2);
        glDrawArrays(GL_LINES, 12, 3*2);
        glLineWidth(0.5f);

        glBindVertexArray(vertexArray_horse);
        glUseProgram(shader_horse);
        glUniformMatrix4fv(Shader_View, 1, GL_TRUE, glm::value_ptr(View));
        glUniformMatrix4fv(Shader_Projection, 1, GL_TRUE, glm::value_ptr(Projection));
        traverse(&nodes[Torso]);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteBuffers(1, &vertexBuffer_grid);
    glDeleteBuffers(2, vertexBuffer_axis);
    glDeleteBuffers(2, vertexBuffer_horse);

    glDeleteVertexArrays(1, &vertexArray_grid);
    glDeleteVertexArrays(1, &vertexArray_axis);
    glDeleteVertexArrays(1, &vertexArray_horse);

    glDeleteProgram(shader_grid);
    glDeleteProgram(shader_axis);
    glDeleteProgram(shader_horse);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    Projection = glm::perspective(fov, (float)width/(float)height, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            c_rotate_xz -= 0.1f;
        }
        else if(action == GLFW_REPEAT)
        {
            c_rotate_xz -= 0.05f;
        }
    }
    else if(key == GLFW_KEY_RIGHT)
    {
        if(action == GLFW_PRESS)
        {
            c_rotate_xz += 0.1f;
        }
        else if(action == GLFW_REPEAT)
        {
            c_rotate_xz += 0.05f;
        }
    }
    else if(key == GLFW_KEY_UP)
    {
        if(action == GLFW_PRESS)
        {
            c_rotate_y += 0.1f;
        }
        else if(action == GLFW_REPEAT)
        {
            c_rotate_y += 0.05f;
        }
    }
    else if(key == GLFW_KEY_DOWN)
    {
        if(c_rotate_y <= 0.0f)
        {
            return;
        }
        if(action == GLFW_PRESS)
        {
            c_rotate_y -= 0.1f;
        }
        else if(action == GLFW_REPEAT)
        {
            c_rotate_y -= 0.05f;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        c_rotate_xz += 0.1f;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}
