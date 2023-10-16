#include "Windowing.h"

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "Icon.h"

#include "MainLogic.h"
#include "Drawer.h"

#ifdef __EMSCRIPTEN__
#include "EmscriptenGlue.h"
#endif

void Windowing::initialize()
{
	glfwInit();

#if defined __EMSCRIPTEN__
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	initializeEmscriptenCanvas();
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

#endif

	window = glfwCreateWindow(screenWidth, screenHeight, u8"Drink Cooling Visualizer - Margitai Péter Máté", NULL, NULL);
	glfwSetWindowUserPointer(window, static_cast<void*>(this));
	
	//glfwSetWindowPosCallback(window, window_pos_callback);
	glfwSetWindowPos(window, 100, 100);

	GLFWimage windowIcon;
	windowIcon.pixels = icon;
	windowIcon.height = 32;
	windowIcon.width = 32;
	glfwSetWindowIcon(window, 1, &windowIcon);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		int a;
		std::cin >> a;
		//return -1;
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		auto self = static_cast<Windowing*>(glfwGetWindowUserPointer(window));
		self->screenWidth = width;
		self->screenHeight = height;
		self->updateViewPort();
		self->getMainLogic()->getDrawer()->updateProjectionMatrix(self->screenWidth - self->imguiWidth, self->screenHeight);
	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) {
		auto self = static_cast<Windowing*>(glfwGetWindowUserPointer(window));

		double cX, cY;
		glfwGetCursorPos(window, &cX, &cY);

		if (cX > self->imguiWidth) {
			self->mainLogic->receiveScrolling(x, y);
		}		
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
		auto self = static_cast<Windowing*>(glfwGetWindowUserPointer(window));

		static bool previousFrame = false;

		if (x < self->imguiWidth || x > self->screenWidth || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			previousFrame = false;
			return;
		}

		static int prevX = x;
		static int prevY = y;

		if (!previousFrame) {
			prevX = x;
			prevY = y;
		}

		int diffX = x - prevX;
		int diffY = y - prevY;

		self->mainLogic->receiveMousDrag(diffX, diffY);

		prevX = x;
		prevY = y;
		previousFrame = true;
	});

#if !defined __EMSCRIPTEN__
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		int a;
		std::cin >> a;
		//return -1;
		return;
	}
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddText(u8"áéíóöőúüű°");

	builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	builder.BuildRanges(&ranges);

	io.Fonts->AddFontFromFileTTF("./resources/Roboto-Medium.ttf", 13, nullptr, ranges.Data);
	io.Fonts->Build();

	//std::cout<<"GL VERSION: "<<glGetString(GL_VERSION)<<std::endl;
	//int major, minor, rev;
	//glfwGetVersion(&major, &minor, &rev);
	//std::cout<<"GLFW - major: "<<major<<", minor: "<<minor<<", rev: "<<rev<<std::endl;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else 
	ImGui_ImplOpenGL3_Init("#version 460 core");
#endif // __EMSCRIPTEN__	

	ImGui::StyleColorsDark();

	glEnable(GL_DEPTH_TEST);
}

void Windowing::terminate()
{
	glfwTerminate();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}

void Windowing::startFrame()
{
	glClearColor(0.45, 0.55, 0.60, 1.00);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
}

void Windowing::endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Windowing::shouldStop()
{
	return glfwWindowShouldClose(window);
}

void Windowing::updateViewPort()
{
	glViewport(imguiWidth, 0, screenWidth - imguiWidth, screenHeight);
}

#ifdef __EMSCRIPTEN__
void Windowing::initializeEmscriptenCanvas()
{
	if (is_mobile())
	{
		screenWidth = screen_get_width() * 0.9f;
		imguiWidth = int(screenWidth * 0.5f);
	}
	else 
	{
		screenWidth = 900;
		imguiWidth = int(screenWidth * 0.35f);
	}
	
	screenHeight = screenWidth * 9 / 16;
}
#endif