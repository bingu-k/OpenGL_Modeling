#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Common.hpp"
#include "MathOP.hpp"

class Camera
{
public:
    static std::unique_ptr<Camera>   Create(const float& width, const float& height);

    ~Camera() {};
    void    init(const float& xPos, const float& yPos, const float& yaw = -90.0f, const float& pitch = 0.0f);
    void    setCameraPos(const glm::vec3& position);
    void    setCameraFront(const float& yaw, const float& pitch);
    void    turnCamera(const float& xPos, const float& yPos);
    void    moveCamera(GLFWwindow *window);
    glm::mat4   getView(const float& aspect, const float& fov = 45.0f, const float& near = 0.1f, const float& far = 50.0f);
    glm::vec3   getPosition(void) { return (this->pos); };
    glm::vec3   getFront(void) { return (this->front); };
private:
    glm::vec3   pos{0.0f, 0.0f, 3.0f}, worldUp{0.0f, 1.0f, 0.0f}, front{0.0f, 0.0f, -1.0f}, up{worldUp};
    float       prevXPos, prevYPos, yaw{-90.0f}, pitch{0.0f};
    float       fov{45.0f}, near{0.01}, far{50.0f};

    Camera() {};
    void    updateCamera(void);
};

std::unique_ptr<Camera>   Camera::Create(const float& xPos, const float& yPos)
{
    std::unique_ptr<Camera> camera = std::unique_ptr<Camera>(new Camera());
    camera->init(xPos, yPos);
    return (std::move(camera));
};

void    Camera::init(const float& xPos, const float& yPos, const float& yaw, const float& pitch)
{
    this->prevXPos = xPos;
    this->prevYPos = yPos;
    this->setCameraFront(yaw, pitch);
};

void    Camera::setCameraPos(const glm::vec3& position)
{ this->pos = position; };

void    Camera::setCameraFront(const float& yaw, const float& pitch)
{
    this->yaw = yaw;
    if (pitch > 89.0f)
        this->pitch = 89.0f;
    else if (pitch < -89.0f)
        this->pitch = -89.0f;
    else
        this->pitch = pitch;
    updateCamera();
};

void    Camera::turnCamera(const float& xPos, const float& yPos)
{
    float   xDelta = xPos - prevXPos;
    float   yDelta = prevYPos - yPos;

    const float sensitivity = 0.1f;
    xDelta *= sensitivity;
    yDelta *= sensitivity;

    this->prevXPos = xPos;
    this->prevYPos = yPos;
    this->setCameraFront(this->yaw + xDelta, this->pitch + yDelta);
};

void    Camera::moveCamera(GLFWwindow *window)
{
    const float Vel = 0.025;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pos += Vel * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pos -= Vel * front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pos += Vel * glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pos -= Vel * glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        pos += Vel * up;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        pos -= Vel * up;
};

glm::mat4   Camera::getView(const float& aspect, const float& fov, const float& near, const float& far)
{
    glm::mat4   projection = MathOP::perspective(glm::radians(fov), aspect, near, far);
    glm::mat4   view = MathOP::lookAt(this->pos, this->pos + this->front, this->up);
    return (projection * view);
};

void    Camera::updateCamera(void)
{
    front = glm::normalize(glm::vec3{cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                                    sin(glm::radians(pitch)),
                                    sin(glm::radians(yaw)) * cos(glm::radians(pitch))});
    up = glm::normalize(glm::cross(glm::normalize(glm::cross(front, worldUp)), front));
};
#endif