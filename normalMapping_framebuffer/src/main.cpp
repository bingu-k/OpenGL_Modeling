#include "../include/Common.hpp"
#include "../include/Program.hpp"
#include "../include/Framebuffer.hpp"
#include "../include/Texture.hpp"
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
    std::unique_ptr<Program>    objectProgram = Program::Create("./shader/normalMap.vert", "./shader/normalMap.frag");
    std::unique_ptr<Program>    FBProgram = Program::Create("./shader/framebuffer.vert", "./shader/framebuffer.frag");

	// Object
	std::unique_ptr<Object>		box = Object::CreateBox();
	std::unique_ptr<Object>		plane = Object::CreatePlane();
	std::unique_ptr<Object>		screen = Object::CreateScreen();

    // Model
    std::unique_ptr<Model>      backpack = Model::LoadModel("./image/backpack/backpack.obj");

    // Texture
	std::unique_ptr<Texture>	floorDiffuse = Texture::Load("./image/gravel_concrete_diff.png", "material.diffuse");
    std::unique_ptr<Texture>	floorNormal = Texture::Load("./image/gravel_concrete_nor.png", "material.normal");
    std::unique_ptr<Texture>	boxDiffuse = Texture::Load("./image/stone_wall_diff.png", "material.diffuse");
    std::unique_ptr<Texture>	boxNormal = Texture::Load("./image/stone_wall_nor.png", "material.normal");

    // FrameBuffer
    std::unique_ptr<FrameBuffer>    framebuffer = FrameBuffer::Create(Texture::Create(width, height, GL_RGB));
    
    FBProgram->Use();
    FBProgram->setUniform(0, "screenTexture");

    // Camera
    double  x, y;
    glfwGetCursorPos(window, &x, &y);
    std::unique_ptr<Camera>     camera = Camera::Create(x, y);
    camera->setCameraPos(glm::vec3(0.0f, 5.0f, 3.0f));
    camera->setCameraFront(-90.0f, -30.0f);
	glfwSetWindowUserPointer(window, camera.get());


    // Lighting Set
    objectProgram->Use();
    objectProgram->setUniform(false, "lightswitch[0]");
    objectProgram->setUniform(false, "lightswitch[1]");
    objectProgram->setUniform(false, "lightswitch[2]");

    // Point Light(고정적)
    glm::vec3   lightPos = glm::vec3(-2.0f, 2.0f, -2.0f);
    glm::vec3   ambient{0.05f, 0.05f, 0.05f}, diffuse{0.5f, 0.5f, 0.5f}, specular{1.0f, 1.0f, 1.0f};
    auto    Plight = Light::CreatePointLight(lightPos, ambient, diffuse, specular);
    Plight->setUniform(objectProgram.get());
    
    // Direction Light(고정적)
    glm::vec3   lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f));
    auto    Dlight = Light::CreateDirectionLight(lightDir, ambient, glm::vec3(0.1f, 0.1f, 0.1f), specular);
    Dlight->setUniform(objectProgram.get());

    // Spot Light
    glm::vec2   cutoff{cos(glm::radians(12.5f)), cos(glm::radians(15.0f))};
    auto    Slight = Light::CreateSpotLight(camera->getPosition(), ambient, diffuse, specular,
                                            camera->getFront(), cutoff);

    while (!glfwWindowShouldClose(window))
    {
        key_manager(window);

        framebuffer->bind();
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4   view = camera->getView(width / height);
        
        // Lighting Box
        lightProgram->Use();
        glm::mat4   model = MathOP::translate(glm::mat4(1.0f), lightPos);
        model = MathOP::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightProgram->setUniform(view * model, "transform");
        box->Draw();

        // Spot Light(유동적)
        Slight->setPos(camera->getPosition());
        Slight->setDir(camera->getFront());
        Slight->setUniform(objectProgram.get());

        // View Pos
        objectProgram->Use();
        objectProgram->setUniform(view, "view");
        objectProgram->setUniform(camera->getPosition(), "viewPos");

        // Floor
        model = MathOP::scale(glm::mat4(1.0f), glm::vec3(30.0f, 1.0f, 30.0f));
        model = MathOP::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        floorDiffuse->bind(0);
        floorNormal->bind(1);
        objectProgram->setUniform(0, floorDiffuse->GetName());
        objectProgram->setUniform(1, floorNormal->GetName());
        objectProgram->setUniform(64.0f, "material.shininess");
        objectProgram->setUniform(model, "model");
		plane->Draw();

        // Object Box
        GLfloat     angle = glfwGetTime();
        model = MathOP::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -2.0f));
        boxDiffuse->bind(0);
        boxNormal->bind(1);
        objectProgram->setUniform(0, boxDiffuse->GetName());
        objectProgram->setUniform(1, boxNormal->GetName());
        objectProgram->setUniform(64.0f, "material.shininess");
        objectProgram->setUniform(model, "model");
		box->Draw();
        
        // Model
        model = MathOP::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, -2.0f));
        objectProgram->setUniform(64.0f, "material.shininess");
        objectProgram->setUniform(model, "model");
        backpack->draw(objectProgram.get());

        glBindVertexArray(0);

        // Frame Buffer;
        framebuffer->bindDefault();
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        framebuffer->GetColorAttachment()->bind(0);
        FBProgram->Use();
        glm::mat4   transform = MathOP::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
        FBProgram->setUniform(transform, "transform");
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