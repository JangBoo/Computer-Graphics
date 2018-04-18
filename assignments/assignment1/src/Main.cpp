// Include standard headers
#include "common/stdafx.h"
#include "Vertices.h"
#include "Horse.h"

GLFWwindow* window;

const std::string TITLE = "Assignment1";
unsigned int WIDTH=800, HEIGHT=800;

const std::string grid_vs = "grid.vs";
const std::string grid_fs = "grid.fs";

const std::string axis_vs = "axis.vs";
const std::string axis_fs = "axis.fs";

const std::string horse_vs = "horse.vs";
const std::string horse_fs = "horse.fs";


const int gridX = 50;
const int gridZ = 50;

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
void reset();
void update_view();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void key_callback1(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int key, int action, int mode);
void window_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    //////////////////////////////////////////////////////////
    //                                                      //
    // initialize a window                                  //
    //                                                      //
    //////////////////////////////////////////////////////////
    if (init_window(WIDTH, HEIGHT, TITLE) != 0)
    {
        return -1;
    }

    //////////////////////////////////////////////////////////
    //                                                      //
    // register callback functions                          //
    //                                                      //
    //////////////////////////////////////////////////////////
    glfwSetKeyCallback(window, key_callback1);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);


    //////////////////////////////////////////////////////////
    //                                                      //
    // enable some feathers before we start to draw         //
    //                                                      //
    //////////////////////////////////////////////////////////
    // Dark grey background
    glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    //////////////////////////////////////////////////////////
    //                                                      //
    // load shaders                                         //
    //                                                      //
    //////////////////////////////////////////////////////////
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


    //////////////////////////////////////////////////////////
    //                                                      //
    // bind date to VAOs, VBOs                              //
    //                                                      //
    //////////////////////////////////////////////////////////
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


        // Projection matrix: 45� Field of View, ration of "1024/768", dispaly range: 0.1 unit <-> 100 units
        Projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        // Or, for an ortho camera:
        // glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates

        // Camera matrix
        View = glm::lookAt(c_pos, c_pos+c_dir, c_up);

        // Model matrix: an identity matrix (model will be at the origin)
        Model = glm::mat4(1.f);

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

        glEnableVertexAttribArray(0);
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

        //////////////////////////////////////////////////////////
        //                                                      //
        // Draw a robot horse                                   //
        //                                                      //
        //////////////////////////////////////////////////////////
        // body
        Model=getHorseModel(BODY,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        glm::mat4 h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.7f,0.7f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // neck
        Model=getHorseModel(NECK,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.75f,0.75f,0.75f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // head
        Model=getHorseModel(HEAD,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.65f,0.65f,0.65f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // front left upper
        Model=getHorseModel(FRONT_LEFT_UPPER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.6f,0.7f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // front left lower
        Model=getHorseModel(FRONT_LEFT_LOWER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.7f,0.6f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // front right upper
        Model=getHorseModel(FRONT_RIGHT_UPPER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.6f,0.7f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // front right lower
        Model=getHorseModel(FRONT_RIGHT_LOWER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.7f,0.6f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // back left upper
        Model=getHorseModel(BACK_LEFT_UPPER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.6f,0.7f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // back left lower
        Model=getHorseModel(BACK_LEFT_LOWER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.7f,0.6f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // back right upper
        Model=getHorseModel(BACK_RIGHT_UPPER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.6f,0.7f,0.7f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // back right lower
        Model=getHorseModel(BACK_RIGHT_LOWER,rotater,rotateOrientation,scaler,moveOnX,moveOnZ);
        h_mvp = Projection * View * Model;
        glUniformMatrix4fv(horse_mvp, 1, GL_FALSE, &h_mvp[0][0]);
        glUniform3f(horse_color, 0.7f,0.6f,0.7f);


        glDrawArrays(GL_TRIANGLES, 0, 36);

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

    //////////////////////////////////////////////////////////
    //                                                      //
    // Cleanup VAOs, VBOs, shaders                          //
    //                                                      //
    //////////////////////////////////////////////////////////
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

void reset()
{
    Model = glm::mat4(1.f);
    //View;
    //Projection;


    // ---- VIEW MATRIX global variables -----
    c_pos = glm::vec3(0, 10, 30); // camera position
    glm::vec3(0.0f, -10.0f, -30.0f); // camera direction
    glm::vec3(0, 1, 0); // tell the camera which way is 'up'

    moveOnX=0,moveOnZ=0;
    scaler=1.0f;
    rotater=0.0f;
    rotateOrientation=glm::vec3(0.0f,1.0f,0.0f);

    yaw=-90.0f;
    pitch=0.0f;
    lastX=WIDTH/2.0f;
    lastY=HEIGHT/2.0f;
    fov=45.0f;//perspective angle
}

void update_view()
{
    View = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    switch(key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_SPACE: // re-position the horse at a random location on the grid
        moveOnX=RANGE_MIN + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(RANGE_MAX-RANGE_MIN)));
        moveOnZ=RANGE_MIN + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(RANGE_MAX-RANGE_MIN)));
        //update_view();
        break;
    case GLFW_KEY_U: // scale up
        scaler+=0.1;
        //update_view();
        break;
    case GLFW_KEY_J: // scale down
        scaler-=0.1;
        //update_view();
        break;
    case GLFW_KEY_A:
        if (mode == GLFW_MOD_SHIFT)//move to left by 1 unit grid
        {
            //uppercase
            moveOnX+=-1.0f;
        }
        else //rotate to left by 5 degrees related to y axis
        {
            //lowercase
            rotater+=5.0f;
            rotateOrientation=glm::vec3(0.0f,1.0f,0.0f);
        }
        //update_view();
        break;
    case GLFW_KEY_D:
        if (mode == GLFW_MOD_SHIFT)  ////move to right by 1 unit grid
        {
            //uppercase
            moveOnX+=1.0f;
        }
        else//rotate to left by 5 degrees related to y axis
        {
            //lowercase
            rotater-=5.0f;
            rotateOrientation=glm::vec3(0.0f,1.0f,0.0f);
        }
        //update_view();
        break;
    case GLFW_KEY_W:
        if (mode == GLFW_MOD_SHIFT)  //move up by 1 unit grid
        {
            //uppercase
            moveOnZ+=-1.0f;
        }
        else  //rise head by 5 degrees
        {
            //lowercase
            rotater-=5.0f;
            rotateOrientation=glm::vec3(0.0f,0.0f,1.0f);
        }
        //update_view();
        break;
    case GLFW_KEY_S:
        if (mode == GLFW_MOD_SHIFT)  //move down by 1 unit grid
        {
            //uppercase
            moveOnZ+=1.0f;
        }
        else  //rise rear by 5 degrees
        {
            //lowercase
            rotater+=5.0f;
            rotateOrientation=glm::vec3(0.0f,0.0f,1.0f);
        }
        //update_view();
        break;
    case GLFW_KEY_LEFT:
        c_pos-=glm::normalize(glm::cross(c_dir, c_up));
        //update_view();
        break;
    case GLFW_KEY_RIGHT:
        c_pos+=glm::normalize(glm::cross(c_dir, c_up));
        //update_view();
        break;
    case GLFW_KEY_UP:
        c_pos.y-=1;
        //update_view();
        break;
    case GLFW_KEY_DOWN:
        c_pos.y+=1;
        //update_view();
        break;
    case GLFW_KEY_HOME:
        //c_pos = glm::vec3(0, 10, 30);
        reset();
        break;
    case GLFW_KEY_P: // point mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        //update_view();
        break;
    case GLFW_KEY_L: // line mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //update_view();
        break;
    case GLFW_KEY_T: // triangles mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //update_view();
        break;
    }
}

void key_callback1(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key==GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if(key==GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        moveOnX=RANGE_MIN + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(RANGE_MAX-RANGE_MIN)));
        moveOnZ=RANGE_MIN + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(RANGE_MAX-RANGE_MIN)));
    }
    else if(key==GLFW_KEY_U && action == GLFW_PRESS)
    {
        scaler+=0.1;
    }
    else if(key==GLFW_KEY_J && action == GLFW_PRESS)
    {
        scaler-=0.1;
    }
    else if(key==GLFW_KEY_A && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)//move to left by 1 unit grid
        {
            //uppercase
            moveOnX+=-1.0f;
        }
        else //rotate to left by 5 degrees related to y axis
        {
            //lowercase
            rotater+=5.0f;
            rotateOrientation=glm::vec3(0.0f,1.0f,0.0f);
        }
    }
    else if(key==GLFW_KEY_D && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)  ////move to right by 1 unit grid
        {
            //uppercase
            moveOnX+=1.0f;
        }
        else//rotate to left by 5 degrees related to y axis
        {
            //lowercase
            rotater-=5.0f;
            rotateOrientation=glm::vec3(0.0f,1.0f,0.0f);
        }
    }
    else if(key==GLFW_KEY_W && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)  //move up by 1 unit grid
        {
            //uppercase
            moveOnZ+=-1.0f;
        }
        else  //rise head by 5 degrees
        {
            //lowercase
            rotater-=5.0f;
            rotateOrientation=glm::vec3(0.0f,0.0f,1.0f);
        }
    }
    else if(key==GLFW_KEY_S && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)  //move down by 1 unit grid
        {
            //uppercase
            moveOnZ+=1.0f;
        }
        else  //rise rear by 5 degrees
        {
            //lowercase
            rotater+=5.0f;
            rotateOrientation=glm::vec3(0.0f,0.0f,1.0f);
        }
        //update_view();
    }
    else if(key==GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        c_pos-=glm::normalize(glm::cross(c_dir, c_up));
    }
    else if(key==GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        c_pos+=glm::normalize(glm::cross(c_dir, c_up));
    }
    else if(key==GLFW_KEY_UP && action == GLFW_PRESS)
    {
        c_pos.y-=1;
    }
    else if(key==GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        c_pos.y+=1;
    }
    else if(key==GLFW_KEY_HOME && action == GLFW_PRESS)
    {
        reset();
    }
    else if(key==GLFW_KEY_P && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    else if(key==GLFW_KEY_L && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if(key==GLFW_KEY_T && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

    xOffset*=0.05f;
    yOffset*=0.05f;

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

void window_size_callback(GLFWwindow* window, int width, int height)
{
    Projection = glm::perspective(fov, (float)width/(float)height, 0.1f, 100.0f);
    WIDTH = width;
    HEIGHT = height;
    // Define the viewport dimensions
    int width_, height_;
    /*This function retrieves the size, in pixels, of the framebuffer of the specified window.*/
    glfwGetFramebufferSize(window, &width_, &height_);
    glViewport(0, 0, width_, height_);//What does this do?
}
