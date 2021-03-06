#include <Gloom/Gloom.h>

#include <libs>

#include <Gloom/Mesh.h>
#include <Gloom/Object.h>
#include <Gloom/Interior.h>
#include <Gloom/Actor.h>
#include <Gloom/Collision.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Material.h>
#include <OpenGL/Shader.h>
#include <OpenGL/RT.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <goo/extra.h>

using namespace gloom;
using namespace glm;

GLFWwindow *window;

bool hideDebugGuis = true;
bool cursorShowing = false;
bool f10 = true;
bool h_pop = false;

namespace gloom
{
	ImFont *font2;
	ImFont *font3;

	void HideCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetCursorPos(window, Camera::prev[0], Camera::prev[1]);
		cursorShowing = false;
		firstPersonCamera->disabled = false;
	}

	void ShowCursor()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorShowing = true;
		firstPersonCamera->disabled = true;
	}
} // namespace gloom

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static bool useFbo = true;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	bool guing = ImGui::IsAnyItemActive();

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		hideDebugGuis = !hideDebugGuis;
		if (!hideDebugGuis)
			ShowCursor();
		else
			HideCursor();
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (firstPersonCamera->disabled)
		{
			cameraCurrent = firstPersonCamera;
			cameraCurrent->disabled = false;
			HideCursor();
		}
		else
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	else if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		if (cursorShowing)
			HideCursor();
		else
			ShowCursor();
		cameraCurrent->disabled = cursorShowing;
	}
	else if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
	{
		useFbo = !useFbo;
	}
	else if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
	{
		printf(" reload esp ! \n");
		// Save current cell name
		Object object(dungeon->loadedCell.record);
		const char *cellName = getEditorId(object);
		char dest[512];
		strcpy(dest, cellName);
		Esp *plugin = get_plugins()[1];
		const char *name = plugin->name;
		Esp *has = has_plugin(name);
		free_plugin(&has);
		get_plugins()[1] = loadPlugin(name);
		delete dungeon;
		dungeon = new Interior(dest);
		dungeon->alreadyTeleported = true;
		dungeon->loadCell();
	}
	else if (key == GLFW_KEY_F6 && action == GLFW_PRESS)
	{
		printf(" reload shaders ! \n");
		SetShaderSources();
		for (auto &pair : Shader::shaders)
		{
			pair.second->Compile();
		}
	}
	else if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
	{
		printf("f10");
		f10 = !f10;
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS && ! guing)
	{
		h_pop = !h_pop;
	}
	else if (key == GLFW_KEY_V && action == GLFW_PRESS && ! guing)
	{
		if (player1)
			player1->toggleView();
	}
}

static void doKeys()
{
	using namespace MyKeys;
	if (!dynamic_cast<FirstPersonCamera *>(cameraCurrent))
		return;
	w = glfwGetKey(window, GLFW_KEY_W);
	a = glfwGetKey(window, GLFW_KEY_A);
	s = glfwGetKey(window, GLFW_KEY_S);
	d = glfwGetKey(window, GLFW_KEY_D);
	r = glfwGetKey(window, GLFW_KEY_R);
	f = glfwGetKey(window, GLFW_KEY_F);
	v = glfwGetKey(window, GLFW_KEY_F);
	shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
	static double x2 = x;
	static double y2 = y;
	cameraCurrent->Mouse((float)(x - x2), (float)(y - y2));
	x2 = x;
	y2 = y;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	gloom::width = width;
	gloom::height = height;
}

void setupImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImFont *font1 = io.Fonts->AddFontDefault();
	font2 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 45.0f);
	font3 = io.Fonts->AddFontFromFileTTF("CrimsonText-Regular.ttf", 60.0f);
	IM_ASSERT(font2 != NULL);
	IM_ASSERT(font3 != NULL);

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	const char *glsl_version = "#version 130";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void gloom::programGo()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if 1
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	width = mode->width;
	height = mode->height;
	window = glfwCreateWindow(width, height, "gloom", glfwGetPrimaryMonitor(), NULL);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#endif

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // vsync

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int er = gladLoadGL();
	if (er == 0)
	{
		printf("glad");
		exit(EXIT_FAILURE);
	}

	setupImgui();

	glClearColor(0, 0, 0, 1);

	glEnable(GL_TEXTURE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);
}

void gloom::doImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	render_stats(&f10);

	if (h_pop)
		hero_menu();

	if (!hideDebugGuis)
	{
		bsa_gui();
		nifp_gui();
		esp_gui();
		cell_gui();
		opengl_gui();

		ImGui::ShowDemoWindow();
	}
}

void gloom::renderImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gloom::programLoop()
{
	renderRarget = new RenderTarget(gloom::width, gloom::height, GL_RGB, GL_FLOAT);
	Quadt quad;

	double fps;
	int frames;
	double time, prevTime;
	char title[150];
	
	double now = 0, prev = 0;

	frames = 0;
	prevTime = glfwGetTime();

	sceneDefault->drawGroups.Add(firstPersonCamera->drawGroup);

	do
	{
		time = glfwGetTime();
		now = time;
		gloom::delta = now - prev;
        prev = now;
		if ((time - prevTime) > 1.0 || frames == 0)
		{
			fps = (double)frames / (time - prevTime);
			sprintf(title, "gloom %.0f fps - f1 for debug - f3 for mouse", fps);
			glfwSetWindowTitle(window, title);
			prevTime = time;
			frames = 0;
			gloom::fps = (int)fps;
		}
		frames++;

		static bool doOnce = true;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, gloom::width, gloom::height);

		if (useFbo)
		{
			renderRarget->Bind();
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		glfwPollEvents();

		doImGui();

		doKeys();

		if (player1)
			player1->step();
			
		cameraCurrent->Update(delta);

		// someDraugr
		if (someDraugr)
			someDraugr->step();

		if (meanSkelly)
			meanSkelly->step();

		if (someHuman)
			someHuman->step();


		dungeon->update();

		//collision_simulate();

		//sceneDefault->Order();
		sceneDefault->DrawItems();

		Material::Unuse(nullptr, nullptr);

		Shader::active = nullptr;

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (useFbo)
		{
			quad.Draw(renderRarget);
		}

		doOnce = false;

		renderImGui();

		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
