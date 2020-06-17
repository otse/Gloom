#include "dark2.h"

#include "opengl/camera"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

using namespace dark2;
using namespace glm;

GLFWwindow *window;

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	static bool F3 = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		F3 = !F3;
		glfwSetInputMode(window, GLFW_CURSOR, F3 ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}

static void do_keys()
{
	camera->w = glfwGetKey(window, GLFW_KEY_W);
	camera->a = glfwGetKey(window, GLFW_KEY_A);
	camera->s = glfwGetKey(window, GLFW_KEY_S);
	camera->d = glfwGetKey(window, GLFW_KEY_D);
	camera->r = glfwGetKey(window, GLFW_KEY_R);
	camera->f = glfwGetKey(window, GLFW_KEY_F);
	camera->shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
	static double xx = x;
	static double yy = y;
	xx = x;
	yy = y;
	camera->Mouse(x - xx, yy - y);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void dark2::program_loop()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Dork", NULL, NULL);

#if 0
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#endif

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int er = gladLoadGL();
	if (!er)
	{
		log_("glad");
		exit(EXIT_FAILURE);
	}

	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		do_keys();
		camera->Move(delta);
		camera->Call();

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
