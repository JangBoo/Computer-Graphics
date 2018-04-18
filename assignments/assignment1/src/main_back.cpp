// Include standard headers
#include "common/stdafx.h"

GLFWwindow* window;

const std::string TITLE = "Assignment1";

const std::string grid_vs = "grid.vs";
const std::string grid_fs = "grid.fs";

const std::string axis_vs = "axis.vs";
const std::string axis_fs = "axis.fs";

const std::string horse_vs = "horse.vs";
const std::string horse_fs = "horse.fs";


const int gridX = 50;
const int gridZ = 50;

glm::mat4 View;



//world space position of different parts of the horse
glm::vec3 bodyPosition=glm::vec3(0.0f,2.5f,0.0f);//body
glm::vec3 fluPosition=glm::vec3(-1.5f,1.5f,-0.5f);//front-left-upper leg
glm::vec3 fllPosition=glm::vec3(-1.5f,0.5f,-0.5f);//front-left-lower leg
glm::vec3 fruPosition=glm::vec3(-1.5f,1.5f,0.5f);//front-right-upper leg
glm::vec3 frlPosition=glm::vec3(-1.5f,0.5f,0.5f);//front-right-lower leg
glm::vec3 bluPosition=glm::vec3(1.5f,1.5f,-0.5f);//back-left-upper leg
glm::vec3 bllPosition=glm::vec3(1.5f,0.5f,-0.5f);//back-left-lower leg
glm::vec3 bruPosition=glm::vec3(1.5f,1.5f,0.5f);//back-right-upper leg
glm::vec3 brlPosition=glm::vec3(1.5f,0.5f,0.5f);//back-right-lower leg
glm::vec3 neckPosition=glm::vec3(-2.5f,3.0f,0.0f);//neck
glm::vec3 headPosition=glm::vec3(-4.0f,3.0f,0.0f);//head

//scale for defferent parts
glm::vec3 bodyScale=glm::vec3(4.0f,1.5f,2.0f);
glm::vec3 legScale=glm::vec3(0.5f,1.0f,0.5f);
glm::vec3 neckScale=glm::vec3(2.5f,1.0f,0.5f);
glm::vec3 headScale=glm::vec3(1.5f,0.5f,0.5f);



enum Horse_Parts{
    BODY, FLU,FLL,FRU,FRL,BLU,BLL,BRU,BRL,NECK,HEAD
};


int initWindow(int width, int height, const std::string title);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat4 drawHorse(Horse_Parts partName, float userRotate, glm::vec3 rotateOri, float userScale, float moveOnX, float moveOnZ);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int key, int action, int mode);

const unsigned int WIDTH=800, HEIGHT=800;
float moveOnX=0,moveOnZ=0;
const float minMove=-40.0f,maxMove=40.0f;
float userScale=1.0f;
float userRotate=0.0f;
glm::vec3 rotateOri=glm::vec3(0.0f,1.0f,0.0f);
float cameraRotate=10.0f;

//camera attributes
float yaw=-90.0f;
float pitch=0.0f;
float lastX=WIDTH/2.0f;//camera first set to the origin
float lastY=HEIGHT/2.0f;//camera first set to the origin
float fov=45.0f;//perspective angle
bool leftMouseButton=false, middleMouseButton=false, rightMouseButton=false;//varibles for detecting the mouse button action
bool firstMouse=true;


// ---- VIEW MATRIX global variables -----
glm::vec3 c_pos = glm::vec3(0,0, 30); // camera position
glm::vec3 c_dir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)); // camera direction
glm::vec3 c_up = glm::vec3(0, 1, 0); // tell the camera which way is 'up'
glm::mat4 view;

void updateView()
{
    view = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}

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


    GLuint axis_shader = loadShaders(axis_vs, axis_fs);
    GLuint axis_mvp = glGetUniformLocation(axis_shader, "MVP");
    GLuint axis_color = glGetUniformLocation(axis_shader, "axis_color");

    GLuint horse_shader = loadShaders(horse_vs, horse_fs);
    GLuint horse_mvp = glGetUniformLocation(horse_shader, "MVP");
    GLuint horse_color = glGetUniformLocation(horse_shader, "horse_color");


    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat buffer_data_grid[] =
    {
        0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 1.f,
        0.f, 0.f, 1.f
    };

    static const GLfloat buffer_data_axis[] =
    {
         0.0f, 0.0f,  0.0f,
         5.0f, 0.0f,  0.0f,
         5.0f, 0.0f,  0.0f,
         4.5f, 0.0f,  0.2f,
         5.0f, 0.0f,  0.0f,
         4.5f, 0.0f, -0.2f,

         0.0f, 0.0f,  0.0f,
         0.0f, 5.0f,  0.0f,
         0.0f, 5.0f,  0.0f,
         0.2f, 4.5f,  0.0f,
         0.0f, 5.0f,  0.0f,
        -0.2f, 4.5f,  0.0f,

         0.0f, 0.0f,  0.0f,
         0.0f, 0.0f,  5.0f,
         0.0f, 0.0f,  5.0f,
         0.2f, 0.0f,  4.5f,
         0.0f, 0.0f,  5.0f,
        -0.2f, 0.0f,  4.5f,
    };

    static const GLfloat buffer_data_horse[] =
    {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };


    GLuint VertexArrayID[3], VertexBufferID[3];
    /*
    generate vertex array object names
    1: Specifies the number of vertex array object names to generate
    &VertexArrayID: Specifies an array in which the generated vertex array object names are stored
    */

    glGenVertexArrays(3, VertexArrayID);

    /*
    glGenBuffers returns "1" buffer object names in "vertexbuffer"
    No buffer objects are associated with the returned buffer object names until they are first bound by calling glBindBuffer.
    1: Specifies the number of buffer object names to be generated
    vertexbuffer: Specifies an array in which the generated buffer object names are stored
    */
    glGenBuffers(3, VertexBufferID);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_axis), buffer_data_axis, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(VertexArrayID[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data_horse), buffer_data_horse, GL_STATIC_DRAW);
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
        glm::mat4 Projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        // Or, for an ortho camera:
        // glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates

        // Camera matrix
        View = glm::lookAt(c_pos, c_pos+c_dir, c_up);

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
                glDrawArrays(GL_LINE_LOOP, 0, 12*3);
            }
        }

        glEnableVertexAttribArray(0);
        //glEnableVertexAttribArray(1);
        glBindVertexArray(VertexArrayID[1]);
        glUseProgram(axis_shader);
        glm::mat4 c_mvp = Projection * View * Model;
        glUniformMatrix4fv(axis_mvp, 1, GL_FALSE, &c_mvp[0][0]);

        glEnable(GL_LINE_SMOOTH);

        glLineWidth(5.0f);
        glUniform3f(axis_color, 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINES, 0, 3*2);
        glUniform3f(axis_color, 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_LINES, 6, 3*2);
        glUniform3f(axis_color, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 12, 3*2);
        glLineWidth(0.5f);

        glEnableVertexAttribArray(0);
        glBindVertexArray(VertexArrayID[2]);
        glUseProgram(horse_shader);

        glm::mat4 h_mvp_base_model = Model;
        //glm::mat4 h_mvp_base = Projection * View * h_mvp_base_model;
        //glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_base[0][0]);
        //glUniform3f(horse_color, 0.7f, 0.5f, 1.0f);
        //glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_lower_arm_1 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(-1.5f, 1.0f, -0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_lower_arm_1[0][0]);
        glUniform3f(horse_color, 0.1f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_lower_arm_2 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(-1.5f, 1.0f, 0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_lower_arm_2[0][0]);
        glUniform3f(horse_color, 0.1f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_lower_leg_1 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(1.5f, 1.0f, -0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_lower_leg_1[0][0]);
        glUniform3f(horse_color, 0.1f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_lower_leg_2 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(1.5f, 1.0f, 0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_lower_leg_2[0][0]);
        glUniform3f(horse_color, 0.1f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);



        glm::mat4 h_mvp_uper_arm_1 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(-1.5f, 3.0f, -0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_uper_arm_1[0][0]);
        glUniform3f(horse_color, 0.7f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_uper_arm_2 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(-1.5f, 3.0f, 0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_uper_arm_2[0][0]);
        glUniform3f(horse_color, 0.7f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_uper_leg_1 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(1.5f, 3.0f, -0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_uper_leg_1[0][0]);
        glUniform3f(horse_color, 0.7f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_uper_leg_2 = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(1.5f, 3.0f, 0.5f)) * glm::scale(h_mvp_base_model, glm::vec3(0.5f,2.0f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_uper_leg_2[0][0]);
        glUniform3f(horse_color, 0.7f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);


        glm::mat4 h_mvp_torso = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(0.0f, 4.0f, 0.f)) * glm::scale(h_mvp_base_model, glm::vec3(4.0f, 1.5f, 2.0f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_torso[0][0]);
        glUniform3f(horse_color, 1.0f, 1.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_neck = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(-2.0f, 4.0f, 0.f)) * glm::rotate(h_mvp_base_model, glm::radians(-30.0f), glm::vec3(0.0f,0.0f,1.0f)) * glm::scale(h_mvp_base_model, glm::vec3(2.0f,0.8f,0.5f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_neck[0][0]);
        glUniform3f(horse_color, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);

        glm::mat4 h_mvp_head = Projection * View * glm::translate(h_mvp_base_model, glm::vec3(-3.0f, 4.0f, 0.f)) * glm::rotate(h_mvp_base_model, glm::radians(70.0f), glm::vec3(0.0f,0.0f,1.0f)) * glm::scale(h_mvp_base_model, glm::vec3(1.0f,0.5f,0.3f));
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp_head[0][0]);
        glUniform3f(horse_color, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);




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
    glDeleteBuffers(1, &VertexBufferID[2]);
    glDeleteProgram(grid_shader);
    glDeleteProgram(axis_shader);
    glDeleteProgram(horse_shader);
    glDeleteVertexArrays(1, &VertexArrayID[0]);
    glDeleteVertexArrays(1, &VertexArrayID[1]);
    glDeleteVertexArrays(1, &VertexArrayID[2]);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if(key==GLFW_KEY_LEFT)//world orientation to right
    {
        c_pos-=glm::normalize(glm::cross(c_dir, c_up));
        updateView();
    }
    if(key==GLFW_KEY_RIGHT)//world orientation to left
    {
        c_pos+=glm::normalize(glm::cross(c_dir, c_up));
        updateView();
    }
    if (key == GLFW_KEY_DOWN)//world orientation Ry
    {
        c_pos.y+=1;
        updateView();
    }
    if (key == GLFW_KEY_UP)//world orientation -Ry
    {
        c_pos.y-=1;
        updateView();
    }
    if(key==GLFW_KEY_TAB)//reset to the initial world position and orientation.because I'm using Mac, which doesn't have "Home" button, I used "tab" instead
    {
        c_pos = glm::vec3(0,0, 30);
        updateView();
    }
    if(key==GLFW_KEY_SPACE&& action == GLFW_PRESS)//randomly change the position of the horse on the grid
    {
        //generating random location in the grid
        moveOnX=minMove + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxMove-minMove)));
        moveOnZ=minMove + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxMove-minMove)));
    }
    if(key==GLFW_KEY_U&&action==GLFW_PRESS)//scale up
    {
        userScale+=0.5;
    }
    if(key==GLFW_KEY_J&&action==GLFW_PRESS)//scale down
    {
        userScale-=0.5;
    }
    if(key==GLFW_KEY_A&&action==GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)//move to left by 1 unit grid
        {
            //uppercase
            moveOnX+=-1.0f;
        }
        else //rotate to left by 5 degrees related to y axis
        {
            //lowercase
            userRotate+=5.0f;
            rotateOri=glm::vec3(0.0f,1.0f,0.0f);
        }
    }
    if(key==GLFW_KEY_D&&action==GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT) {////move to right by 1 unit grid
            //uppercase
            moveOnX+=1.0f;
        }
        else//rotate to left by 5 degrees related to y axis
        {
            //lowercase
            userRotate-=5.0f;
            rotateOri=glm::vec3(0.0f,1.0f,0.0f);
        }
    }
    if(key==GLFW_KEY_W&&action==GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT) {//move up by 1 unit grid
            //uppercase
            moveOnZ+=-1.0f;
        }
        else {//rise head by 5 degrees
            //lowercase
            userRotate-=5.0f;
            rotateOri=glm::vec3(0.0f,0.0f,1.0f);
        }
    }
    if(key==GLFW_KEY_S&&action==GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT) {//move down by 1 unit grid
            //uppercase
            moveOnZ+=1.0f;
        }
        else {//rise rear by 5 degrees
            //lowercase
            userRotate+=5.0f;
            rotateOri=glm::vec3(0.0f,0.0f,1.0f);
        }
    }
    if(key==GLFW_KEY_P&&action==GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);//point mode
    }
    if(key==GLFW_KEY_L&&action==GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//point mode
    }

    if(key==GLFW_KEY_T&&action==GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//wireframe mode
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


glm::mat4 drawHorse(Horse_Parts partName, float userRotate, glm::vec3 rotateOri, float userScale, float moveOnX, float moveOnZ)
{
    glm::mat4 model;
    if(partName==BODY)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(bodyPosition[0]+moveOnX,bodyPosition[1],bodyPosition[2]+moveOnZ));
        model=glm::scale(model, bodyScale);
    }
    if(partName==FLU)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(fluPosition[0]+moveOnX,fluPosition[1],fluPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==FLL)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(fllPosition[0]+moveOnX,fllPosition[1],fllPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==FRU)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(fruPosition[0]+moveOnX,fruPosition[1],fruPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==FRL)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(frlPosition[0]+moveOnX,frlPosition[1],frlPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==BLU)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(bluPosition[0]+moveOnX,bluPosition[1],bluPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==BLL)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(bllPosition[0]+moveOnX,bllPosition[1],bllPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==BRU)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(bruPosition[0]+moveOnX,bruPosition[1],bruPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==BRL)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(brlPosition[0]+moveOnX,brlPosition[1],brlPosition[2]+moveOnZ));
        model=glm::scale(model, legScale);
    }
    if(partName==NECK)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(neckPosition[0]+moveOnX,neckPosition[1],neckPosition[2]+moveOnZ));
        model=glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f,0.0f,1.0f));//neck rotation
        model=glm::scale(model, neckScale);
    }
    if(partName==HEAD)
    {
        model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(userRotate), rotateOri);
        model=glm::scale(model, glm::vec3(userScale,userScale,userScale));
        model=glm::translate(model, glm::vec3(headPosition[0]+moveOnX,headPosition[1],headPosition[2]+moveOnZ));
        model=glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f));//head rotation
        model=glm::scale(model, headScale);
    }
    return model;
}

//call back funtion for mouse button and movement
void mouse_button_callback(GLFWwindow* window, int key, int action, int mode)
{
    if(key==GLFW_MOUSE_BUTTON_RIGHT&&action==GLFW_PRESS)
    {
        rightMouseButton=true;
    }
    if(key==GLFW_MOUSE_BUTTON_MIDDLE&&action==GLFW_PRESS)
    {
        middleMouseButton=true;
    }
    if(key==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_PRESS)
    {
        leftMouseButton=true;
    }
    if(key==GLFW_MOUSE_BUTTON_RIGHT&&action==GLFW_RELEASE)
    {
        rightMouseButton=false;
    }
    if(key==GLFW_MOUSE_BUTTON_MIDDLE&&action==GLFW_RELEASE)
    {
        middleMouseButton=false;
    }
    if(key==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_RELEASE)
    {
        leftMouseButton=false;
    }
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)//keep track of the position of the cursor
{
    if(firstMouse)
    {
        lastX=xPos;
        lastY=yPos;
        firstMouse=false;
    }

    float xOffset=xPos-lastX;
    float yOffset=lastY-yPos;
    lastX=xPos;
    lastY=yPos;

    float sensitivity=0.05f;
    xOffset*=sensitivity;
    yOffset*=sensitivity;

    if(rightMouseButton)//yaw
    {
        yaw+=xOffset;
    }
    if(middleMouseButton)//pitch
    {
        pitch+=yOffset;
        if(pitch>89.0f)
        {
            pitch=89.0f;
        }
        if(pitch<-89.0f)
        {
            pitch=-89.0f;
        }
    }
    if(leftMouseButton)//zoom in and out by adjusting the fov degree
    {
        if(fov>=1.0f&&fov<=45.0f)
            fov-=yOffset*0.1;
        if(fov<=1.0f)
            fov=1.0f;
        if(fov>=45.0f)
            fov=45.0f;

    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    c_dir = glm::normalize(front);
}
