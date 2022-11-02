#include "../include/Common.hpp"
#include "../include/Program.hpp"
#include "../include/Texture.hpp"
#include "../include/Camera.hpp"
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
    
    std::unique_ptr<Program>    program = Program::Create("./shader/shader.vert"
                                                        , "./shader/shader.frag");
	// Object
	std::unique_ptr<Object>		plane = Object::CreatePlane();
	std::unique_ptr<Object>		box = Object::CreateBox();

    // Texture
	std::unique_ptr<Texture>	texture = Texture::Load("./image/lenna.png", "textrue_id");

    // Camera
    double  x, y;
    glfwGetCursorPos(window, &x, &y);
    std::unique_ptr<Camera>     camera = Camera::Create(x, y);
	glfwSetWindowUserPointer(window, camera.get());

    // 렌더링 루프
    while (!glfwWindowShouldClose(window))
    {
        key_manager(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		texture->bind(0);

        GLfloat     angle = glfwGetTime();
        glm::mat4   view = camera->getView(width / height);
        glm::mat4   model = MathOP::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
        model = MathOP::rotate(model, angle, glm::vec3(1.0f, 0.5f, 0.3f));
        
        program->Rendering();
        program->setUniform(0, texture->GetName());
        program->setUniform(view * model, "transform");
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