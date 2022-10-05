#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"

#include <iostream>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(float deltaTime);

const unsigned int	SCR_WIDTH = 800;
const unsigned int	SCR_HEIGHT = 600;

GLFWwindow* window = nullptr;
Camera      camera(glm::vec3(0.0f, 0.0f, 3.0f));

void	initWindow(void)
{
    if (!glfwInit())
        throw ("glfwInit() failed!\n");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
        throw ("Failed to create GLFW window\n");
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw ("Failed to initialize GLAD\n");

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
};

void    rendering(void)
{
    Shader  ourShader("shader.vs", "shader.fs");
    Model   ourModel(std::filesystem::path(std::filesystem::current_path()).string() + "\\backpack\\backpack.obj");
    float   lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        processInput(static_cast<float>(glfwGetTime()) - lastTime);
        lastTime = static_cast<float>(glfwGetTime());

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4   projection = glm::perspective(glm::radians(camera.Zoom),
                                                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                0.1f, 100.0f);
        glm::mat4   view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4   model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
};

int main(void)
{
    try
    {
        initWindow();
        rendering();
    }
    catch (const std::string& err)
    {
        std::cerr << err << '\n';
        glfwTerminate();
        return (1);
    }
    return (0);
}

void processInput(float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    static float    lastX = SCR_WIDTH / 2.0f;
    static float    lastY = SCR_HEIGHT / 2.0f;
    static bool     firstMouse = true;

    float    xpos = static_cast<float>(xposIn);
    float    ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}