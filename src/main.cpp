#include "../include/Common.hpp"
#include "../include/Program.hpp"
#include "../include/Texture.hpp"
#include "../include/Camera.hpp"
#include "../include/Light.hpp"
#include "../include/Object.hpp"
#include "../include/MathOP.hpp"

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
    std::unique_ptr<Program>    objectProgram = Program::Create("./shader/specularMap.vert", "./shader/specularMap.frag");

	// Object
	std::unique_ptr<Object>		box = Object::CreateBox();
	std::unique_ptr<Object>		plane = Object::CreatePlane();

    // Texture
	std::unique_ptr<Texture>	texDiffuse = Texture::Load("./image/container2.png", "material.diffuse");
    std::unique_ptr<Texture>	texSpecular = Texture::Load("./image/container2_specular.png", "material.specular");
    objectProgram->Use();
    objectProgram->setUniform(0, texDiffuse->GetName());
    objectProgram->setUniform(1, texSpecular->GetName());
    objectProgram->setUniform(64.0f, "material.shininess");

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4   view = camera->getView(width / height);
        
        // Lighting Box
        glm::vec3   lightPos = glm::vec3(0.0f, 4.0f, 0.0f);
        lightProgram->Use();
        glm::mat4   model = MathOP::translate(glm::mat4(1.0f), lightPos);
        model = MathOP::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightProgram->setUniform(view * model, "transform");
        box->Draw();

        // Lighting Component
        objectProgram->Use();
        objectProgram->setUniform(true, "lightswitch[0]");
        objectProgram->setUniform(true, "lightswitch[1]");
        objectProgram->setUniform(true, "lightswitch[2]");

        // Point Light
        glm::vec3   ambient{0.05f, 0.05f, 0.05f}, diffuse{0.5f, 0.5f, 0.5f}, specular{1.0f, 1.0f, 1.0f};
        objectProgram->setUniform(lightPos, "pointLight.position");
        objectProgram->setUniform(ambient, "pointLight.ambient");
        objectProgram->setUniform(diffuse, "pointLight.diffuse");
        objectProgram->setUniform(specular, "pointLight.specular");
        
        // Direction Light
        glm::vec3   lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f));
        objectProgram->setUniform(lightDir, "directionLight.direction");
        objectProgram->setUniform(ambient, "directionLight.ambient");
        objectProgram->setUniform(glm::vec3(0.2f, 0.2f, 0.2f), "directionLight.diffuse");
        objectProgram->setUniform(specular, "directionLight.specular");
        
        // Spot Light
        glm::vec2   cutoff{cos(glm::radians(12.5f)), cos(glm::radians(15.0f))};
        objectProgram->setUniform(camera->getPosition(), "spotLight.position");
        objectProgram->setUniform(camera->getFront(), "spotLight.direction");
        objectProgram->setUniform(cutoff, "spotLight.cutoff");
        objectProgram->setUniform(ambient, "spotLight.ambient");
        objectProgram->setUniform(diffuse, "spotLight.diffuse");
        objectProgram->setUniform(specular, "spotLight.specular");

        // View Pos
        objectProgram->setUniform(view, "view");
        objectProgram->setUniform(camera->getPosition(), "viewPos");
        texDiffuse->bind(0);
        texSpecular->bind(1);

        // Floor
        model = MathOP::scale(glm::mat4(1.0f), glm::vec3(30.0f, 1.0f, 30.0f));
        model = MathOP::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        objectProgram->setUniform(model, "model");
		plane->Draw();

        // Object Box
        GLfloat     angle = glfwGetTime();
        model = MathOP::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -2.0f));
        objectProgram->setUniform(model, "model");
		box->Draw();

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