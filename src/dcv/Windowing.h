#ifndef WINDOWING_H
#define WINDOWING_H

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#else 
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <memory>

class MainLogic;

class Windowing {

private:
	int screenWidth = 1280;
	int screenHeight = 720;
	int imguiWidth = 400;

	GLFWwindow* window;

	std::shared_ptr<MainLogic> mainLogic;
public:
	~Windowing()
	{
		mainLogic = nullptr;
	}

	void initialize();
	void terminate();

	void startFrame();
	void endFrame();

	bool shouldStop();
	void updateViewPort();

	float getScreenWidth() { return this->screenWidth; }
	float getScreenHeight() { return this->screenHeight; }
	int getImguiWidth() { return this->imguiWidth; }	

	void setMainLogic(std::shared_ptr<MainLogic> mainLogic)
	{
		this->mainLogic = mainLogic;
	}

	std::shared_ptr<MainLogic> getMainLogic()
	{
		return this->mainLogic;
	}

	GLFWwindow* getGLFWWindow()
	{
		return this->window;
	}

#ifdef __EMSCRIPTEN__
	void initializeEmscriptenCanvas();
#endif
};

#endif