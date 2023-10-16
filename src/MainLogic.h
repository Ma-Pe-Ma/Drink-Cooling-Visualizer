#ifndef MAINLOGIC_H
#define MAINLOGIC_H

#include <vector>
#include <memory>

#include "DrinkCooling.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <implot/implot.h>
#include <implot/implot_internal.h>

class Windowing;
class Drawer;

class MainLogic {
private:
	std::vector<std::shared_ptr<DrinkCooling>> drinkCoolings;
	std::weak_ptr<DrinkCooling> selected;
	ImVec2 windowSize;
	std::shared_ptr<Windowing> windowing;

	std::shared_ptr<Drawer> drawer;
	bool collapseConfiguration = false;

public:
	void initialize();
	void update();
	void terminate();

	void setWindowing(std::shared_ptr<Windowing> windowing)
	{
		this->windowing = windowing;
	}

	std::shared_ptr<Drawer> getDrawer() { return this->drawer; }

	~MainLogic()
	{
		this->windowing = nullptr;
	}

	void receiveScrolling(double x, double y);
	void receiveMousDrag(int x, int y);
};

#endif