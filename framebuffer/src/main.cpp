#include "../include/Common.hpp"
#include "../include/Program.hpp"
#include "../include/Texture.hpp"
#include "../include/FrameBuffer.hpp"
#include "../include/Camera.hpp"
#include "../include/Light.hpp"
#include "../include/Object.hpp"
#include "../include/MathOP.hpp"
#include "../include/Model.hpp"

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
    
    std::unique_ptr<Program>    lightProgram = Program::Create("./shader/light.vert", "./shader/light.frag");
    std::unique_ptr<Program>    textureProgram = Program::Create("./shader/texture.vert", "./shader/texture.frag");
    std::unique_ptr<Program>    FBProgram = Program::Create("./shader/framebuffer.vert", "./shader/framebuffer.frag");
    std::unique_ptr<Program>    InverseProgram = Program::Create("./shader/framebuffer.vert", "./shader/inverse.frag");
    std::unique_ptr<Program>    grayProgram = Program::Create("./shader/framebuffer.vert", "./shader/grayscale.frag");
    std::unique_ptr<Program>    KEProgram = Program::Create("./shader/framebuffer.vert", "./shader/kernel_effect.frag");
    std::unique_ptr<Program>    modelProgram = Program::Create("./shader/model.vert", "./shader/model.frag");

	// Object
	std::unique_ptr<Object>		box = Object::CreateBox();
	std::unique_ptr<Object>		plane = Object::CreatePlane();
	std::unique_ptr<Object>		screen = Object::CreateScreen();

    // Model
    std::unique_ptr<Model>      backpack = Model::LoadModel("./image/backpack/backpack.obj");

    // Texture
    std::unique_ptr<Texture>    floor = Texture::Load("./image/stone_wall_diff.png", "tex");
    std::unique_ptr<Texture>    wall = Texture::Load("./image/container2.png", "tex");
	
    // FrameBuffer
    std::unique_ptr<FrameBuffer>    framebuffer = FrameBuffer::Create(Texture::Create(width, height, GL_RGB));
    
    FBProgram->Use();
    FBProgram->setUniform(0, "screenTexture");
    InverseProgram->Use();
    InverseProgram->setUniform(0, "screenTexture");
    grayProgram->Use();
    grayProgram->setUniform(0, "screenTexture");
    KEProgram->Use();
    KEProgram->setUniform(0, "screenTexture");

    // Camera
    double  x, y;
    glfwGetCursorPos(window, &x, &y);
    std::unique_ptr<Camera>     camera = Camera::Create(x, y);
    camera->setCameraPos(glm::vec3(0.0f, 5.0f, 3.0f));
    camera->setCameraFront(-90.0f, -30.0f);
	glfwSetWindowUserPointer(window, camera.get());

    // 렌더링 루프
    while (!glfwWindowShouldClose(window))
    {
        key_manager(window);

        framebuffer->bind();
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View Pos
        glm::mat4   view = camera->getView(width / height);
        textureProgram->setUniform(view, "view");
        textureProgram->setUniform(camera->getPosition(), "viewPos");
        
        // Floor
        textureProgram->Use();
        glm::mat4   model = MathOP::scale(glm::mat4(1.0f), glm::vec3(30.0f, 1.0f, 30.0f));
        model = MathOP::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        textureProgram->setUniform(view * model, "transform");
        floor->bind(0);
        textureProgram->setUniform(0, floor->GetName());
        plane->Draw();

        // Model
        modelProgram->Use();
        model = MathOP::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -2.0f));
        modelProgram->setUniform(view * model, "transform");
        backpack->draw(modelProgram.get());

        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        framebuffer->GetColorAttachment()->bind(0);
        // Basic Framebuffer
        FBProgram->Use();
        glm::mat4   transform = MathOP::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 0.0f));
        FBProgram->setUniform(transform, "transform");
        screen->Draw();

        // Inverse Framebuffer
        InverseProgram->Use();
        transform = MathOP::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
        InverseProgram->setUniform(transform, "transform");
        screen->Draw();

        // Gray Scale Framebuffer
        grayProgram->Use();
        transform = MathOP::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
        grayProgram->setUniform(transform, "transform");
        screen->Draw();

        // Kernel Effect Framebuffer
        KEProgram->Use();
        transform = MathOP::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
        KEProgram->setUniform(transform, "transform");
        screen->Draw();

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