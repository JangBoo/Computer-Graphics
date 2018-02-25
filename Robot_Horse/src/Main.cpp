#include <iostream>

#include <GL/glew.h>	    // include GL Extension Wrangler
#include <GLFW/glfw3.h>     // include GLFW helper library

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_gl.h>

#include "Vertices.h"

const std::string TITLE = "RobotHorse";
const unsigned int WIDTH=1920, HEIGHT=1080;

const std::string grid_vs = "shaders/grid.vs";
const std::string grid_fs = "shaders/grid.fs";

const std::string axis_vs = "shaders/axis.vs";
const std::string axis_fs = "shaders/axis.fs";

const std::string horse_vs = "shaders/horse.vs";
const std::string horse_fs = "shaders/horse.fs";

const int gridX = 50;
const int gridZ = 50;

GLFWwindow* window;

glm::mat4 Model = glm::mat4(1.f);
glm::mat4 View;
glm::mat4 Projection;


// ---- VIEW MATRIX global variables -----
glm::vec3 c_pos = glm::vec3(0, 10, 30); // camera position
glm::vec3 c_dir = glm::vec3(0.0f, -10.0f, -30.0f); // camera direction
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

int main()
{

    if (init_window(WIDTH, HEIGHT, TITLE) != 0)
    {
        return -1;
    }

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // build and compile our shader zprogram
    // ------------------------------------
    GLuint grid_shader = loadShaders(grid_vs, grid_fs);
    GLuint grid_mvp = glGetUniformLocation(grid_shader, "MVP");

    GLuint axis_shader = loadShaders(axis_vs, axis_fs);
    GLuint axis_mvp = glGetUniformLocation(axis_shader, "MVP");

    GLuint horse_shader = loadShaders(horse_vs, horse_fs);
    GLuint horse_mvp = glGetUniformLocation(horse_shader, "MVP");


    GLuint VertexArrayID[3], VertexBufferID[6];
    glGenVertexArrays(3, VertexArrayID);
    glGenBuffers(6, VertexBufferID);

    glBindVertexArray(VertexArrayID[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_grid), buffer_data_grid, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VertexArrayID[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_axis), buffer_data_axis, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_axis), buffer_data_axis, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VertexArrayID[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_horse), buffer_data_horse, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    // Model matrix: an identity matrix (model will be at the origin)
    Model = glm::mat4(1.0f);

    // Camera matrix
    View = glm::lookAt(c_pos, c_pos+c_dir, c_up);

    // Projection matrix: 45� Field of View, ration of "1024/768", dispaly range: 0.1 unit <-> 100 units
    Projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
    // Or, for an ortho camera:
    // glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates


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
        glBindVertexArray(VertexArrayID[0]);
        glUseProgram(grid_shader);

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
        glBindVertexArray(VertexArrayID[1]);
        glUseProgram(axis_shader);
        glm::mat4 c_mvp = Projection * View * Model;
        glUniformMatrix4fv(axis_mvp, 1, GL_FALSE, &c_mvp[0][0]);

        glEnable(GL_LINE_SMOOTH);

        glLineWidth(5.0f);
        glDrawArrays(GL_LINES, 0, 3*2);
        glDrawArrays(GL_LINES, 6, 3*2);
        glDrawArrays(GL_LINES, 12, 3*2);
        glLineWidth(0.5f);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteBuffers(1, &VertexBufferID[0]);
    glDeleteBuffers(1, &VertexBufferID[1]);
    glDeleteBuffers(1, &VertexBufferID[2]);
    glDeleteBuffers(1, &VertexBufferID[3]);
    glDeleteBuffers(1, &VertexBufferID[4]);
    glDeleteBuffers(1, &VertexBufferID[5]);
    glDeleteProgram(grid_shader);
    glDeleteProgram(axis_shader);
    glDeleteProgram(horse_shader);
    glDeleteVertexArrays(1, &VertexArrayID[0]);
    glDeleteVertexArrays(1, &VertexArrayID[1]);
    glDeleteVertexArrays(1, &VertexArrayID[2]);

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
	glViewport(0, 0, width, height);//What does this do?
}
