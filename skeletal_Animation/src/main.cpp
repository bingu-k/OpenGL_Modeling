#include "../include/Common.hpp"
#include "../include/Program.hpp"
#include "../include/Framebuffer.hpp"
#include "../include/Texture.hpp"
#include "../include/Camera.hpp"
#include "../include/Light.hpp"
#include "../include/Object.hpp"
#include "../include/MathOP.hpp"
#include "../include/AniModel.hpp"
#include "../include/Animator.hpp"

using namespace std;

void    framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

void    key_manager(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    auto	camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->moveCamera(window);
};

void    mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    auto	camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->turnCamera(xPos, yPos);
};

GLFWwindow* init_window(void)
{
    if (!glfwInit())
        throw string("Error: Failed to initialize GLFW");
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(width, height, WINDOW_NAME, nullptr, nullptr);
    if (window == NULL)
        throw string("Error: Failed to create GLFW window");
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw string("Error: Failed to initialize GLAD");
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    return (window);
};

int main_process()
{
    GLFWwindow* window = init_window();

    // Floor
    std::unique_ptr<Program>    objectProgram = Program::Create("./shader/normalMap.vert", "./shader/normalMap_point.frag");
    std::unique_ptr<Texture>    floordiffuse = Texture::Load("./image/stone_wall_diff.png","material.diffuse");
    std::unique_ptr<Texture>    floornormal = Texture::Load("./image/stone_wall_nor.png","material.normal");
    std::unique_ptr<Object>     floor = Object::CreatePlane();

    // Lighting
    std::unique_ptr<Program>    lightProgram = Program::Create("./shader/light.vert", "./shader/light.frag");
    std::unique_ptr<Light>      light = Light::CreatePointLight(glm::vec3(1.0f), glm::vec3(0.1f), glm::vec3(0.5f), glm::vec3(1.0f));
    std::unique_ptr<Object>     lightBox = Object::CreateBox();
    glm::vec4                   lightColor(1.0f, 1.0f, 1.0f, 1.0f);

    light->setUniform(objectProgram.get(), 0);
    objectProgram->setUniform(1, "numPointLight");

    // Animation Model
    std::unique_ptr<Program>    skeleton = Program::Create("./shader/animation.vert", "./shader/animation.frag");
    std::unique_ptr<AniModel>   vampire = AniModel::LoadModel("./image/vampire/dancing_vampire.dae");
    Animation   danceingAnimation("./image/vampire/dancing_vampire.dae", vampire.get());
    Animator    animator(&danceingAnimation);

    // Camera
    double  x, y;
    glfwGetCursorPos(window, &x, &y);
    std::unique_ptr<Camera>     camera = Camera::Create(x, y);
    camera->setCameraPos(glm::vec3(0.0f, 1.5f, 3.0f));
    camera->setCameraFront(-90.0f, -15.0f);
	glfwSetWindowUserPointer(window, camera.get());

    float   deltaTime = 0.0f, lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float   currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        key_manager(window);

        animator.UpdateAnimation(deltaTime);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4   view = camera->getView(width / height);
        glm::mat4   model = glm::mat4(1.0f);

        // Lighting Box
        lightProgram->Use();
        model = glm::translate(glm::mat4(1.0f), light->GetPosition());
        model = glm::scale(model, glm::vec3(0.1));
        lightProgram->setUniform(view * model, "transform");
        lightProgram->setUniform(lightColor, "lightColor");
        lightBox->Draw();

        // Floor
        objectProgram->Use();

        floordiffuse->bind(0);
        floornormal->bind(1);
        objectProgram->setUniform(0, floordiffuse->GetName());
        objectProgram->setUniform(1, floornormal->GetName());
        objectProgram->setUniform(64.0f, "material.shininess");

        model = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 1.0f, 30.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objectProgram->setUniform(view, "view");
        objectProgram->setUniform(model, "model");

        objectProgram->setUniform(camera->getPosition(), "viewPos");
        floor->Draw();

        skeleton->Use();
        skeleton->setUniform(view, "view");

        auto    transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
            skeleton->setUniform(transforms[i], name);
        }
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        skeleton->setUniform(model, "model");
        vampire->draw(skeleton.get());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return (0);
};

int main(void)
{
    try
    {
        return main_process();
    }
    catch(const string& err)
    {
        std::cerr << err << '\n';
        glfwTerminate();
        return -1;
    }
};