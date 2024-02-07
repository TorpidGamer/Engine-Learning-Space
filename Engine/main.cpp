#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Scene.h"

//Prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInputs(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
float Lerp(float a, float b, float t);

//Settings
const unsigned int SCR_START_WIDTH = 1920;
const unsigned int SCR_START_HEIGHT = 1080;
unsigned int SCR_WIDTH = SCR_START_WIDTH, SCR_HEIGHT = SCR_START_HEIGHT;
glm::mat4 projectionMatrix = glm::mat4(1.f);
int timeSteps = 4;

//Global Variables
bool rerollRNG = false;
bool rPressed = false;
Shader* ourShaderPointer;
Renderer renderer;
ResourceManager rm;
Scene baseScene;
bool gravity = false;
bool clearBuffer = true;

//Time Variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Camera Variables
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_START_WIDTH/2.0f, lastY = SCR_START_HEIGHT/2.0f;
bool firstMouse = true;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
   // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_START_WIDTH, SCR_START_HEIGHT, "OPENGL FULLY REALISED", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // set rand seed
    srand(time(0));
    // set opengl states
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader = Shader("shader.vs", "shader.frag");
    ourShaderPointer = &ourShader;

    //Model backpack = rm.CreateResource<Model>("models/backpack/backpack.obj");
    Model crash = rm.CreateResource<Model>("models/crash/crash.obj");
    Model cube = rm.CreateResource<Model>("models/Cube.glb");
    Model mechabandicoot = rm.CreateResource<Model>("models/mechabandicoot/mech.obj");
    cube.meshes[0].textures.push_back(rm.CreateResource<Texture>("../Common/resources/wall.jpg"));
    //cube.meshes[0].scale = glm::vec3(20, 1, 20);
    cube.meshes[0].scale = glm::vec3(1, 1, 1);
    //backpack.position = glm::vec3(10, 2, 0);
    crash.position = glm::vec3(1, -1.f, 1);
    cube.position = glm::vec3(0, -2, 0);

    baseScene = Scene();
    //baseScene.SceneObjects["Backpack"] = new GameObject(&backpack);
    //baseScene.SceneObjects["Crash"] = new GameObject(new Model("models/crash/crash.obj"));
    //baseScene.SceneObjects["Crash"]->rb.position = glm::vec3(0, 0, 2500);
    //baseScene.SceneObjects["Crash"]->rb.velocity = glm::vec3(1600, 0, 0);
    //baseScene.SceneObjects["Crash"]->rb.mass = 10;

    baseScene.SceneObjects["Crash2"] = new GameObject(new Model("models/crash/crash.obj"));
    baseScene.SceneObjects["Crash2"]->rb.position = glm::vec3(0, 0, 0);
    baseScene.SceneObjects["Crash2"]->rb.velocity = glm::vec3(400, 0, 0);
    baseScene.SceneObjects["Crash2"]->rb.mass = 10;

    baseScene.SceneObjects["Mechabandicoot"] = new GameObject(&mechabandicoot);
    baseScene.SceneObjects["Mechabandicoot"]->rb.position = glm::vec3(0, 0, 250);
    baseScene.SceneObjects["Mechabandicoot"]->rb.mass = 2000;
    baseScene.SceneObjects["Mechabandicoot"]->rb.isStatic = true;
   /* baseScene.SceneObjects["Floor"] = new GameObject(&cube);
    baseScene.SceneObjects["Floor"]->rb.mass = 10;*/

    projectionMatrix = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH/SCR_HEIGHT, 0.01f, 10000.f);


    glm::vec3 lightPos = glm::vec3(10, 2, 5);
    ourShader.use();
    ourShader.setVec3("lightPos", lightPos);
    baseScene.GetRigidBodies();
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // TIME LOGIC
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        ProcessInputs(window);
        // clear previous render
        // ------
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        if (clearBuffer)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ourShader.use();

        // camera transformations
        glm::mat4 viewMatrix = glm::mat4(1.f);
        viewMatrix = camera.GetViewMatrix();
        ourShader.setMat4("vp", projectionMatrix * viewMatrix);
        ourShader.setVec3("cameraPos", camera.Position);

        //Update
        if (gravity)
            baseScene.UpdateScene(deltaTime);

        lightPos = baseScene.SceneObjects["Crash2"]->rb.position;
        ourShader.setVec3("lightPos", lightPos);

        //Render 
        baseScene.DrawScene(ourShader);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
    projectionMatrix = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH / SCR_HEIGHT, 0.01f, 10000.f);
}

float Lerp(float a, float b, float t) {
    return a + t*(b - a);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_G && action == GLFW_RELEASE)
        gravity = !gravity;

    if (key == GLFW_KEY_R && action == GLFW_RELEASE)
        clearBuffer = !clearBuffer;
}

void ProcessInputs(GLFWwindow* window) {
    
    camera.MovementSpeed = 500.f; // adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((float)(yoffset));
}