#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSL_helper.h"
#include "MStackHelp.h"

#define _USE_MATH_DEFINES
using namespace std;
using namespace glm;

  ////////////////////
 ////  GLOBALS  /////
////////////////////

// Parameters
unsigned int const StepSize = 16;
unsigned int WindowWidth = 800, WindowHeight = 600;

// Variable Handles
GLuint aPosition;
GLuint aNormal;
GLuint uModelMatrix;
GLuint uNormalMatrix;
GLuint uViewMatrix;
GLuint uProjMatrix;
GLuint uColor;
GLuint uLightDir;
GLuint uLightCol;
GLuint uCamPos;
GLuint uMatAmb;
GLuint uMatDif;
GLuint uMatSpec;
GLuint uMatShine;

GLuint CubeBuffObj, CIndxBuffObj, RIndxBuffObj, NormalBuffObj,
       GrndBuffObj, GIndxBuffObj, GNBuffObj, GNIndxBuffObj;
int g_CiboLen, g_GiboLen;

// Shader Handle
GLuint ShadeProg;


// Program Variables
static const float g_groundSize = 100.0;
float GrndPos[] = { 
    0, 0, 0,
    g_groundSize, 0, 0,
    g_groundSize, 0, 10,
    0, 0, 10
};
float GrndNorm[] = { 
    0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
    0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
    0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
    0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
};
unsigned short GrndIdx[] = {2, 1, 0,  2, 0, 3};

bool mvLft, mvRgt;

vec3 camPos, playerPos;

RenderingHelper ModelTrans;


float lightPosV[] = {30.f, 30.f, 30.f,  50.f, 80.f, 0.f,  0.f, 80.f, -100.f,  -200.f, 80.f, -100.f};
float lightColV[] = {.8f, .8f, .70f,  .4f, .4f, .43f,  .4f, .4f, .43f,  .4f, .4f, .43f};

//////////////////

void SetMaterial(int i) {
    glUseProgram(ShadeProg);
    switch (i) {
        case 1: //white player
            glUniform3f(uMatAmb, 0.4, 0.4, 0.45);
            glUniform3f(uMatDif, 0.8, 0.8, 0.9);
            glUniform3f(uMatSpec, 0.3, 0.3, 0.4);
            glUniform1f(uMatShine, 4.0);
            break;
        case 4: //ground green
            glUniform3f(uMatAmb, 0.01, 0.05, 0.02);
            glUniform3f(uMatDif, 0.1, 0.5, 0.2);
            glUniform3f(uMatSpec, 0.1, 0.5, 0.2);
            glUniform1f(uMatShine, 1.0);
            break;
    }
}

void SetProjectionMatrix()
{
    glm::mat4 Projection = glm::perspective(80.0f, ((float) WindowWidth)/ ((float)WindowHeight), 0.1f, 100.f);
    safe_glUniformMatrix4fv(uProjMatrix, glm::value_ptr(Projection));
}

void SetView()
{
    glm::mat4 View = glm::lookAt(camPos, playerPos, vec3(0.f, 1.f, 0.f));
    safe_glUniformMatrix4fv(uViewMatrix, glm::value_ptr(View));
}

void SetModelI() {
    glm::mat4 tmp = glm::mat4(1.0f);
    safe_glUniformMatrix4fv(uModelMatrix, glm::value_ptr(tmp));
}

void SetModel()
{
    safe_glUniformMatrix4fv(uModelMatrix, glm::value_ptr(ModelTrans.modelViewMatrix));
    safe_glUniformMatrix4fv(uNormalMatrix, glm::value_ptr(glm::transpose(glm::inverse(ModelTrans.modelViewMatrix))));
}

static void initGround() {
   
    g_GiboLen = 6;
    glGenBuffers(1, &GrndBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

    glGenBuffers(1, &GIndxBuffObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GrndIdx), GrndIdx, GL_STATIC_DRAW);

    glGenBuffers(1, &GNBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GNBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

}

static void initCube() {

    float CubePos[] = {
        -0.5, -0.5, -0.5,  -0.5, 0.5, -0.5,  0.5, 0.5, -0.5,   0.5, -0.5, -0.5,  
        -0.5, -0.5, 0.5,   -0.5, 0.5, 0.5,   0.5, 0.5, 0.5,    0.5, -0.5, 0.5, 
        -0.5, -0.5, 0.5,   -0.5, -0.5, -.5,  -0.5, 0.5, -0.5,  -0.5, 0.5, 0.5, 
        0.5, -0.5, 0.5,    0.5, -0.5, -.5,   0.5, 0.5, -0.5,   0.5, 0.5, 0.5,                        
    };

    float CubeNormal[] = {
        //back//
        0, 0, -1,   0, 0, -1,   0, 0, -1,  0, 0, -1,
        //front//
        0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1,
        //left//
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,
        //right//
        1, 0, 0,  1, 0, 0,   1, 0, 0,  1, 0, 0,
    };


 
    unsigned short idx[] = {0, 1, 2,  2, 3, 0,  4, 5, 6,  6, 7, 4,  8, 9, 10,  10, 11, 8,  12, 13, 14,  14, 15, 12};

    g_CiboLen = 24;
    glGenBuffers(1, &CubeBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, CubeBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubePos), CubePos, GL_STATIC_DRAW);

    glGenBuffers(1, &NormalBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, NormalBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeNormal), CubeNormal, GL_STATIC_DRAW);

    glGenBuffers(1, &CIndxBuffObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CIndxBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

}

bool InstallShader(std::string const & vShaderName, std::string const & fShaderName)
{
    GLuint VS; // handles to shader object
    GLuint FS; // handles to frag shader object
    GLint vCompiled, fCompiled, linked; // status of shader

    VS = glCreateShader(GL_VERTEX_SHADER);
    FS = glCreateShader(GL_FRAGMENT_SHADER);

    // load the source
    char const * vSource = textFileRead(vShaderName);
    char const * fSource = textFileRead(fShaderName);
    glShaderSource(VS, 1, & vSource, NULL);
    glShaderSource(FS, 1, & fSource, NULL);

    // compile shader and print log
    glCompileShader(VS);
    printOpenGLError();
    glGetShaderiv(VS, GL_COMPILE_STATUS, & vCompiled);
    printShaderInfoLog(VS);

    // compile shader and print log
    glCompileShader(FS);
    printOpenGLError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, & fCompiled);
    printShaderInfoLog(FS);

    if (! vCompiled || ! fCompiled)
    {
        std::cerr << "Error compiling either shader " << vShaderName << " or " << fShaderName << std::endl;
        return false;
    }

    // create a program object and attach the compiled shader
    ShadeProg = glCreateProgram();
    glAttachShader(ShadeProg, VS);
    glAttachShader(ShadeProg, FS);

    glLinkProgram(ShadeProg);

    // check shader status requires helper functions
    printOpenGLError();
    glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
    printProgramInfoLog(ShadeProg);

    glUseProgram(ShadeProg);

    // get handles to attribute data
    aPosition   = safe_glGetAttribLocation(ShadeProg, "aPosition");
    aNormal     = safe_glGetAttribLocation(ShadeProg, "aNormal");
    uProjMatrix     = safe_glGetUniformLocation(ShadeProg, "uProjMatrix");
    uViewMatrix     = safe_glGetUniformLocation(ShadeProg, "uViewMatrix");
    uModelMatrix    = safe_glGetUniformLocation(ShadeProg, "uModelMatrix");
    uNormalMatrix   = safe_glGetUniformLocation(ShadeProg, "uNormalMatrix");
    uLightDir = safe_glGetUniformLocation(ShadeProg, "uLightDir");
    uLightCol = safe_glGetUniformLocation(ShadeProg, "uLightCol");
    uCamPos = safe_glGetUniformLocation(ShadeProg, "uCamPos");
    uMatAmb = safe_glGetUniformLocation(ShadeProg, "uMat.aColor");
    uMatDif = safe_glGetUniformLocation(ShadeProg, "uMat.dColor");
    uMatSpec = safe_glGetUniformLocation(ShadeProg, "uMat.sColor");
    uMatShine = safe_glGetUniformLocation(ShadeProg, "uMat.shine");


    std::cout << "Sucessfully installed shader " << ShadeProg << std::endl;
    return true;
}

void Initialize()
{
    glClearColor(0.8f, 0.8f, 1.0f, 1.0f);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    ModelTrans.useModelViewMatrix();
    ModelTrans.loadIdentity();
}

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(ShadeProg);

    SetProjectionMatrix();
    SetView();

    glUniform3fv(uLightDir, 4, lightPosV);
    glUniform3fv(uLightCol, 4, lightColV);


    ////CAMERA////
    glUniform3f(uCamPos, camPos.x, camPos.y, camPos.z);

    safe_glEnableVertexAttribArray(aPosition);
    safe_glEnableVertexAttribArray(aNormal);

    ////GROUND////
    SetModelI();
    SetMaterial(4);

    glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
    safe_glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, GNBuffObj);
    safe_glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
    glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);


    ModelTrans.loadIdentity();
    ModelTrans.translate(playerPos);
    SetModel();
    SetMaterial(1);

    glBindBuffer(GL_ARRAY_BUFFER, CubeBuffObj);
    safe_glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, NormalBuffObj);
    safe_glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CIndxBuffObj);
    glDrawElements(GL_TRIANGLES, g_CiboLen, GL_UNSIGNED_SHORT, 0);

    safe_glDisableVertexAttribArray(aPosition);
    safe_glDisableVertexAttribArray(aNormal);
    glUseProgram(0);
    printOpenGLError();
}

void Reshape(GLFWwindow* window, int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_A:
                mvRgt = true;
                break;
            case GLFW_KEY_D:
                mvLft = true;
                break;
            case GLFW_KEY_Q:
                exit(EXIT_SUCCESS);
                break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_A:
                mvRgt = false;
                break;
            case GLFW_KEY_D:
                mvLft = false;
                break;
        }
    }
}

void computeMovement() {
    if (mvLft) {
        camPos.x += .1f;     
        playerPos.x += .1f;
    }
    if (mvRgt) {
        camPos.x -= .1f;     
        playerPos.x -= .1f;
    }
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main(int argc, char *argv[])
{
    // Initialize Global Variables
    mvLft = mvRgt = false;
    playerPos = vec3(0.f, .5f, 5.f);
    camPos = playerPos + vec3(0, 2.f, 10.f);

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(WindowWidth, WindowHeight, "move bitch", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, Reshape);

    // OpenGL Setup
    Initialize();
    getGLversion();

    // Shader Setup
    if (! InstallShader("Diffuse.vert", "Diffuse.frag")) {
        printf("Error installing shader!\n");
        return 1;
    }

    initGround();
    initCube();

    while (!glfwWindowShouldClose(window)) {
        computeMovement();
        Draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
