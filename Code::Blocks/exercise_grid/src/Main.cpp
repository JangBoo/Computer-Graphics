// Include standard headers
#include "common/stdafx.h"

GLFWwindow* window;
const unsigned int WIDTH=800, HEIGHT=600;
const std::string TITLE = "Assignment1";

const std::string grid_vs = "grid.vs";
const std::string grid_fs = "grid.fs";


const std::string coordinate_vs = "coordinate.vs";
const std::string coordinate_fs = "coordinate.fs";


const int gridX = 50;
const int gridZ = 50;

glm::vec3 c_pos = glm::vec3(0, 10, 10);
glm::vec3 c_dir = glm::vec3(0, 0, 0);
glm::vec3 c_up = glm::vec3(0, 1, 0);
glm::mat4 View;

int initWindow(int width, int height, const std::string title);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
    if (initWindow(WIDTH, HEIGHT, TITLE) != 0)
    {
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);

    // Dark blue background
    glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    // Create and compile our GLSL program from the shaders
    GLuint grid_shader = loadShaders(grid_vs, grid_fs);

    // Rtrieve the location of the uniform variable within the specified program object, so we can set the value
    GLuint grid_mvp = glGetUniformLocation(grid_shader, "MVP");


    GLuint coordinate_shader = loadShaders(coordinate_vs, coordinate_fs);

    // Rtrieve the location of the uniform variable within the specified program object, so we can set the value
    GLuint coordinate_mvp = glGetUniformLocation(coordinate_shader, "MVP");


    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat buffer_data_grid[] =
    {
        0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 1.f,
        0.f, 0.f, 1.f
    };

    static const GLfloat buffer_data_coordinate[] =
    {
         0.0f, 0.0f,  0.0f,
         3.0f, 0.0f,  0.0f,
         3.0f, 0.0f,  0.0f,
         2.5f, 0.0f,  0.2f,
         3.0f, 0.0f,  0.0f,
         2.5f, 0.0f, -0.2f,

         0.0f, 0.0f,  0.0f,
         0.0f, 3.0f,  0.0f,
         0.0f, 3.0f,  0.0f,
         0.2f, 2.5f,  0.0f,
         0.0f, 3.0f,  0.0f,
        -0.2f, 2.5f,  0.0f,

         0.0f, 0.0f,  0.0f,
         0.0f, 0.0f,  3.0f,
         0.0f, 0.0f,  3.0f,
         0.2f, 0.0f,  2.5f,
         0.0f, 0.0f,  3.0f,
        -0.2f, 0.0f,  2.5f,
    };


    GLuint VertexArrayID[2], VertexBufferID[2];
    /*
    generate vertex array object names
    1: Specifies the number of vertex array object names to generate
    &VertexArrayID: Specifies an array in which the generated vertex array object names are stored
    */

    glGenVertexArrays(2, VertexArrayID);

    /*
    glGenBuffers returns "1" buffer object names in "vertexbuffer"
    No buffer objects are associated with the returned buffer object names until they are first bound by calling glBindBuffer.
    1: Specifies the number of buffer object names to be generated
    vertexbuffer: Specifies an array in which the generated buffer object names are stored
    */
    glGenBuffers(2, VertexBufferID);

    /*
    binds the vertex array object with name "VertexArrayID". "VertexArrayID" is the name of a vertex array object previously returned from a call to glGenVertexArrays
    VertexArrayID: Specifies the name of the vertex array to bind
    */
    glBindVertexArray(VertexArrayID[0]);
    /*
    Specifies the target to which the buffer object is bound. The symbolic constant must be GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
    */
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[0]);
    /*
    creates and initializes a buffer object's data store
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_grid), buffer_data_grid, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glBindVertexArray(VertexArrayID[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_coordinate), buffer_data_coordinate, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    do
    {

        // Clear the screen
        /*
        sets the bitplane area of the window to values previously selected by glClearColor, glClearIndex, glClearDepth, glClearStencil, and glClearAccum.
        glClear takes a single argument that is the bitwise OR of several values indicating which buffer is to be cleared.
        The values are as follows:
        GL_COLOR_BUFFER_BIT		Indicates the buffers currently enabled for color writing.
        */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        // so every time we would render in the same way


        // Projection matrix: 45° Field of View, ration of "1024/768", dispaly range: 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1024.f / 768.f, 0.1f, 100.f);
        // Or, for an ortho camera:
        // glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates

        // Camera matrix
        View = glm::lookAt(c_pos, c_dir, c_up);

        // Model matrix: an identity matrix (model will be at the origin)
        glm::mat4 Model = glm::mat4(1.f);

        // Our ModelViewProjection: multiplication of our 3 matrices
        glm::mat4 MVP = Projection * View * Model;

        /*
        By default, all client-side capabilities are disabled, including all generic vertex attribute arrays.
        If enabled, the values in the generic vertex attribute array will be accessed and used for rendering when calls are made to vertex array commands such as
        glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.
        */
        glEnableVertexAttribArray(0);
        glBindVertexArray(VertexArrayID[0]);
        glUseProgram(grid_shader);

        for(int i=1; i<=gridX; ++i){
            glm::mat4 anchor_x1 = MVP * glm::translate(glm::mat4(1.0f), glm::vec3(i-1, 0.f, 0.f));
            for(int j=1; j<=gridZ; ++j){
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
            for(int j=1; j<=gridZ; ++j){
                glm::mat4 anchor_1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, j-1));
                glm::mat4 mvp1 = anchor_x2 * anchor_1;
                glUniformMatrix4fv(grid_mvp, 1, GL_FALSE, &mvp1[0][0]);
                glDrawArrays(GL_LINE_LOOP, 0, 4*2);

                glm::mat4 anchor_2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -j));
                glm::mat4 mvp2 = anchor_x2 * anchor_2;
                glUniformMatrix4fv(grid_mvp, 1, GL_FALSE, &mvp2[0][0]);
                glDrawArrays(GL_LINE_LOOP, 0, 4*2);
            }
        }

        glEnableVertexAttribArray(0);
        //glEnableVertexAttribArray(1);
        glBindVertexArray(VertexArrayID[1]);
        glUseProgram(coordinate_shader);
        glm::mat4 c_mvp = Projection * View * Model;
        glUniformMatrix4fv(coordinate_mvp, 1, GL_FALSE, &c_mvp[0][0]);

        glEnable(GL_LINE_SMOOTH);

        glLineWidth(5.0f);
        glDrawArrays(GL_LINES, 0, 9*2);
        glLineWidth(0.5f);

        glDisableVertexAttribArray(0);
        //glDisableVertexAttribArray(1);

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
    glDeleteBuffers(1, &VertexBufferID[0]);
    glDeleteBuffers(1, &VertexBufferID[1]);
    glDeleteProgram(grid_shader);
    glDeleteProgram(coordinate_shader);
    glDeleteVertexArrays(1, &VertexArrayID[0]);
    glDeleteVertexArrays(1, &VertexArrayID[1]);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_DOWN) {
        c_pos.y += 1;
		c_pos.z += 1;
		View = glm::lookAt(c_pos, c_dir, c_up);
	}

	if (key == GLFW_KEY_UP) {
		c_pos.y -= 1;
		c_pos.z -= 1;
		View = glm::lookAt(c_pos, c_dir, c_up);
	}

}

int initWindow(int width, int height, const std::string title)
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
