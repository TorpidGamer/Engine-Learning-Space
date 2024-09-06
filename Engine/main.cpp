#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneCollection.hpp"
#include "CollisionHandler.h"

//Prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInputs(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void SceneGeneration();
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
Scene* scene;
Renderer renderer;
ResourceManager rm;
bool play = false;
bool toggle = false;
bool clearBuffer = true;
bool debugCollisions = true;

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

    /*TestScene scene = TestScene();
    scene = *scene.ConstructScene(rm);
    int l = 0;
    for (auto& i : scene.SceneObjects)
    {
        l++;
    }
    std::cout << "there are " << l << " objects on init" << std::endl;*/
    SceneGeneration();
    
    projectionMatrix = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH/SCR_HEIGHT, 0.01f, 10000.f);


    glm::vec3 lightPos = glm::vec3(10, 2, 5);
    ourShader.use();
    ourShader.setVec3("lightPos", lightPos);
    scene->GetRigidBodies();
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

        // update
        if (play)
        {
            scene->UpdateScene(deltaTime);
            int collisionTests = 0;
            for (auto& x : scene->SceneObjects)
            {   
                x.second->rb->colliding = false;
                for (auto& y : scene->SceneObjects)
                {
                    if (x == y) continue;
                    CollisionDetails collision = IsOverlapped(x.second, y.second);
                    collisionTests++;
                    if (collision.overlapped)
                    {
                        x.second->rb->colliding = true;
                        y.second->rb->colliding = true;
                        if (debugCollisions)
                        {
                            std::cout << x.first << " Collided with " << y.first << std::endl;
                            std::cout << "Collision Details: " << std::endl;
                            glm::vec3 xPos = x.second->model->position;
                            glm::vec3 yPos = y.second->model->position;
                            std::cout << x.first << " is at " << xPos.x << ", " << xPos.y << ", " << xPos.z << " & "
                                << y.first << " is at " << yPos.x << ", " << yPos.y << ", " << yPos.z << std::endl;
                            std::cout << "Normal: " << collision.normal.x << ", " << collision.normal.y << ", " << collision.normal.z << ", " << std::endl;
                            std::cout << "Depth: " << collision.depth << std::endl;
                            std::cout << "Type: " << collision.collisionType << " at " << collision.number << std::endl;
                            std::cout << std::endl;
                        }
                        glm::vec3 rV = x.second->rb->velocity - y.second->rb->velocity; 
                        //Relative Velocity
                        float impulse = -(1 + 1.0f) * glm::dot(rV, collision.normal * collision.depth) / ((glm::pow(x.second->rb->mass, -1)) + (glm::pow(y.second->rb->mass, -1))); 
                        //impulse = -(1 + e)dot(Vr N)
                        //e (coefficient of restitution) how elastic the collision is

                        if (!x.second->rb->isStatic) 
                            x.second->rb->velocity = x.second->rb->velocity - (float)glm::pow(x.second->rb->mass, -1) * impulse * (collision.normal * collision.depth);
                        if (!y.second->rb->isStatic)
                            y.second->rb->velocity = y.second->rb->velocity + (float)glm::pow(y.second->rb->mass, -1) * impulse * (collision.normal * collision.depth);
                            
                        std::cout << "Collision" << std::endl;
                    }
                }
            }
            //std::cout << "There were " << collisionTests << " collision tests" << std::endl;
            if (toggle) play = false;
        }

        lightPos = camera.Position;
        ourShader.setVec3("lightPos", lightPos);

        //Render 
        scene->DrawScene(ourShader);
        
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

void SceneGeneration()
{
    scene = new Scene();
    //Model backpack = rm.CreateResource<Model>("models/backpack/backpack.obj");
    Model *cube = rm.CreateResource<Model>("models/Cube.glb");
    cube->meshes[0].textures.push_back(*rm.CreateResource<Texture>("../Common/resources/wall.jpg"));
    for (int i = 0; i < cube->meshes.size(); i++)
    {
        cube->meshes[i].scale = glm::vec3(50, 20, 50);
    }
    //backpack.position = glm::vec3(10, 2, 0);
    //.SceneObjects["Backpack"] = new GameObject(&backpack);

    scene->SceneObjects["Crash"] = new GameObject(new Model("models/crash/crash.obj"), "Crash");
    scene->SceneObjects["Crash"]->model->position = glm::vec3(-40, 100, 0);
    scene->SceneObjects["Crash"]->rb->velocity = glm::vec3(0.1f, -0.7f, 0);
    scene->SceneObjects["Crash"]->rb->mass = 1;

    scene->SceneObjects["Crash2"] = new GameObject(new Model("models/crash/crash.obj"), "Crash2");
    GameObject* crash = scene->SceneObjects["Crash2"];
    for (int i = 0; i < crash->model->meshes.size(); i++)
    {
        crash->model->meshes[i].scale = glm::vec3(1, 2, 1);
    }
    scene->SceneObjects["Crash2"]->model->position = glm::vec3(120, 100, 0);
    scene->SceneObjects["Crash2"]->rb->velocity = glm::vec3(0.f, 0, 0);
    scene->SceneObjects["Crash2"]->rb->mass = 1;

    //SceneObjects["Mechabandicoot"] = new GameObject(new Model("models/mechabandicoot/mech.obj"), "Mechacoot");
    //SceneObjects["Mechabandicoot"]->rb->position = glm::vec3(0, 0, 0);
    //SceneObjects["Mechabandicoot"]->rb->mass = 200000;
    //SceneObjects["Mechabandicoot"]->rb->isStatic = true;

    scene->SceneObjects["Floor"] = new GameObject(cube, "Floor");
    scene->SceneObjects["Floor"]->rb->isStatic = true;
    scene->SceneObjects["Floor"]->rb->mass = 10000;
    scene->SceneObjects["Floor"]->model->position = glm::vec3(0, -10, 0);
    for (auto& i : scene->SceneObjects)
    {
        i.second->rb->collisionModel = i.second->rb->GenerateCollisionShape(i.second->model);
        i.second->UpdateRigidBodyPositions();
        std::cout << "LOADED: " << i.first << std::endl;
    }
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

    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        play = !play;

    if (key == GLFW_KEY_H && action == GLFW_RELEASE)
        toggle = !toggle;

    if (key == GLFW_KEY_R && action == GLFW_RELEASE)
        clearBuffer = !clearBuffer;

    if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
        SceneGeneration();

    if (key == GLFW_KEY_1 && action == GLFW_RELEASE)
    {
        for (auto x : scene->SceneObjects)
        {
            x.second->drawCollisionModel = !x.second->drawCollisionModel;
        }
    }

}

void ProcessInputs(GLFWwindow* window) {
    
    camera.MovementSpeed = 500.f; // adjust accordingly
    if (play)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            scene->SceneObjects["Crash"]->rb->velocity += camera.Front * 2.f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            scene->SceneObjects["Crash"]->rb->velocity += camera.Front * -2.f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            scene->SceneObjects["Crash"]->rb->velocity += camera.Right * -2.f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            scene->SceneObjects["Crash"]->rb->velocity += camera.Right * 2.f * deltaTime;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
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