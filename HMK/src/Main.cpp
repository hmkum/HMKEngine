#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "Utility.h"

std::function<void(int, int, int, int)> keyCallback;
std::function<void(double, double)> cursorPosCallback;
std::function<void(int, int, int)> mouseButtonCallback;
static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void CursorPosCallback(GLFWwindow *window, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void ErrorCallback(int error, const char *description)
{
	std::cerr << description << std::endl;
	HMK_LOG_ERROR(description);
}

void APIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, 
	const GLchar *message, const void *userParam)
{
	std::string errorMessage = "GL ";

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:
		errorMessage += "LOW ->\t";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		errorMessage += "MEDIUM ->\t";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		errorMessage += "HIGH ->\t";
		break;
	default:
		break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		errorMessage += "ERROR:\t";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		errorMessage += "DEPRECATED BEHAVIOR:\t";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		errorMessage += "UNDEFINED BEHAVIOR:\t";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		errorMessage += "PERFORMANCE:\t";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		errorMessage += "PORTABILITY:\t";
		break;
	default:
		errorMessage += "OTHER:\t";
		break;
	}

	errorMessage += message;

	HMK_LOG_ERROR(errorMessage);
	HMK_PRINT(errorMessage);
}

int main()
{
	hmk::Logger::Inst().Initialize("engine.txt");
	if (glfwInit() == GL_FALSE)
	{
		HMK_LOG_ERROR("failed glfwInit")
		hmk::Logger::Inst().Shutdown();
		return -1;
	}

	glfwSetErrorCallback(ErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	auto window = glfwCreateWindow(800, 600, "HMK", nullptr, nullptr);
	if (window == nullptr)
	{
		HMK_LOG_ERROR("failed glfwCreateWindow")
		hmk::Logger::Inst().Shutdown();
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 500, 30);
	glfwMakeContextCurrent(window);

	if (gl3wInit() == -1) // 0 success
	{
		HMK_LOG_ERROR("failed gl3wInit")
		hmk::Logger::Inst().Shutdown();
		glfwTerminate();
		return -1;
	}

	if (!gl3wIsSupported(3, 3))
	{
		HMK_LOG_ERROR("Upgrade your graphic card!")
		hmk::Logger::Inst().Shutdown();
		glfwTerminate();
		return -1;
	}

#if _DEBUG
	if (glDebugMessageCallback)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glErrorCallback, nullptr);
		GLuint unused = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unused, true);
	}
#endif

	Game *game = new Game();
	game->Init();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	keyCallback = HMK_CALLBACK_4(Game::KeyInput, game);
	cursorPosCallback = HMK_CALLBACK_2(Game::CursorPosInput, game);
	mouseButtonCallback = HMK_CALLBACK_3(Game::MouseButtonInput, game);

	double lastTime = glfwGetTime();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // https://www.opengl.org/wiki/Cubemap_Texture#Seamless_cubemap
	glClearColor(0.2f, 0.3f, 0.2f, 1.0f);

	double fps = 0.0;
	double acc = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double now = glfwGetTime();
		double delta = now - lastTime;
		lastTime = now;

		glfwPollEvents();
		game->Update((float)delta);
		game->Render();

		fps++;
		acc += delta;
		if (acc > 1.0)
		{
			acc = 0.0;
			fps = 0.0;
		}

		glfwSwapBuffers(window);
	}

	delete game;
	hmk::Logger::Inst().Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	keyCallback(key, scancode, action, mods);
}

static void CursorPosCallback(GLFWwindow *window, double xPos, double yPos)
{
	cursorPosCallback(xPos, yPos);
}

static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	mouseButtonCallback(button, action, mods);
}