#include <iostream>
#include <sstream>
#include <iomanip>

#if defined __EMSCRIPTEN__
	#include <GLES3/gl3.h>
	#include <emscripten/emscripten.h>
#else 
	#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "DrinkCooling.h"
#include "Drawing.h"

#include "Icon.h"
#include <cmath>

#define WIDTH 1280
#define HEIGHT 720

int SCR_WIDTH = WIDTH;
int SCR_HEIGHT = HEIGHT;

std::string floatToStringWithPrecision(float number, int precision = 2) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << number;
	return stream.str();
}

void framebuffer_size_callback(GLFWwindow* window, int x, int y) {
	SCR_WIDTH = x;
	SCR_HEIGHT = y;
	glViewport(0, 0, x, y);
	Drawing::updateProjectionMatrix(x, y);
}

std::vector<std::shared_ptr<DrinkCooling>> drinkCoolings;
std::weak_ptr<DrinkCooling> selected;
GLFWwindow* window;
std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();
ImVec2 windowSize(400, SCR_HEIGHT);

void update(GLFWwindow *window);

int main() {
	glfwInit();
	//glfwSetErrorCallback();

#if defined __EMSCRIPTEN__
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	
#endif
		
	window = glfwCreateWindow(WIDTH, HEIGHT, u8"Drink Cooling Visualizer - Margitai Péter Máté", NULL, NULL);
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
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#if !defined __EMSCRIPTEN__
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		int a;
		std::cin >> a;
		return -1;
	}
#endif
	//std::cout<<"GL VERSION: "<<glGetString(GL_VERSION)<<std::endl;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddText(u8"áéíóöőúüű°");

	builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	builder.BuildRanges(&ranges);

	io.Fonts->AddFontFromFileTTF("./resources/Roboto-Medium.ttf", 13, nullptr, ranges.Data);

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

	Drawing::initializeClass();

	glEnable(GL_DEPTH_TEST);

	prevFrameStart = std::chrono::system_clock::now();

	auto startTime = std::chrono::system_clock::now();

#if defined __EMSCRIPTEN__
	emscripten_set_main_loop_arg((em_arg_callback_func)update, window, 60, 1);
#else
	while (!glfwWindowShouldClose(window)) {
		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - prevFrameStart;

		if (work_time.count() < 1000.0f / 30) {
			std::chrono::duration<double, std::milli> delta_ms(1000.0f / 30 - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			//std::cout<<"Sleep for: "<<delta_ms_duration.count()<<std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();

		update(window);
	}
#endif

	glfwTerminate();

	for (auto it = drinkCoolings.begin(); it != drinkCoolings.end(); it++) {
		(*it)->stop();
	}

	return 0;
}

void update(GLFWwindow* window) {	
	glClearColor(0.45, 0.55, 0.60, 1.00);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	auto selectedShared = selected.lock();

	if (selectedShared.get()) {
		selectedShared->visualize();
	}		

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	static ImVec2 windowPosition(0, 0);
	ImGui::SetNextWindowPos(windowPosition);
	//static ImVec2 windowSize(400, SCR_HEIGHT);
	windowSize = ImVec2(400, SCR_HEIGHT);
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("Drink Cooling Visualizer", nullptr, ImGuiWindowFlags_NoResize);		
	
	static int currentListElement = 0;

	ImVec2 listSize(0, 60);
	if (ImGui::BeginListBox(u8"Processes", listSize)) {
		for (auto it = drinkCoolings.begin(); it != drinkCoolings.end(); ++it) {
			int index = std::distance(drinkCoolings.begin(), it);

			if (ImGui::Selectable((*it)->getName().c_str(), (*it) == selected.lock())) {
				selected = *it;
				(*it)->getDrawing()->determineCameraMatrix();
			}				
		}

		ImGui::EndListBox();
	}

	ImGui::SameLine();
	if (ImGui::Button("+")) {
		drinkCoolings.push_back(std::make_shared<DrinkCooling>());
	}
	
	ImGui::NewLine();		

	if (selectedShared.get() != nullptr) {
		ImGui::PushItemWidth(100);

		int currentState = selectedShared->getCurrentState();

		auto nameS = "Selected: " + selectedShared->getName();
		ImGui::Text(nameS.c_str());

		std::string currentStateString = "Current state: " + selectedShared->getCurrentStateString(currentState);
		ImGui::Text(currentStateString.c_str());
		if (currentState == 0) {
			ImGui::SameLine();
			if (ImGui::Button("Start")) {
				selectedShared->launchProcess();
				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			}
		}
		else if (currentState == 1) {
			selectedShared->checkProcesFinish();
		}

		if (currentState > 0) {
			ImGui::BeginDisabled();
		}

		//ImGui::NewLine();

		ImGui::Text("Ending condition");
		ImGui::RadioButton("Target temperature", selectedShared->getEndTypePointer(), 0); ImGui::SameLine();
		ImGui::RadioButton("Timespan", selectedShared->getEndTypePointer(), 1);

		ImGui::Text("Geometry properties");

		int* radiusNr = selectedShared->getGeometricProperties()->getRadiusNrGUI();
		if (ImGui::InputInt("Radius discretization number [-]", radiusNr, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (*radiusNr < 0) {
				*radiusNr = 1;
			}

			selectedShared->updateGeometry();
		}

		int* axisNr = selectedShared->getGeometricProperties()->getAxisNrGUI();
		if (ImGui::InputInt("Axis discretization number [-]", axisNr, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (*axisNr < 0) {
				*axisNr = 1;
			}

			selectedShared->updateGeometry();
		}
		
		float* radiuslength = selectedShared->getGeometricProperties()->getRadiusLengthGUI();
		if (ImGui::InputFloat("Radius length [mm]", radiuslength, 0.0f, 50.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (*radiuslength < 0) {
				*radiuslength = 1;
			}
			
			selectedShared->updateGeometry();
		}

		float* axisLength = selectedShared->getGeometricProperties()->getAxisLengthGUI();
		if (ImGui::InputFloat("Axis length [mm]", axisLength, 0.0f, 100.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (*axisLength < 0) {
				*axisLength = 1;
			}

			selectedShared->updateGeometry();
		}

		int* sectionAngle = selectedShared->getGeometricProperties()->getSectionAngleGUI();
		if (ImGui::InputInt(u8"Section angle [°]", sectionAngle, 0, 180, ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (*sectionAngle < 0) {
				*sectionAngle = 1;
			}
			else if (*sectionAngle > 180) {
				*sectionAngle = 180;
			}
			
			selectedShared->updateGeometry();
		}

		MaterialProperties* materialProperties = selectedShared->getMaterialProperties();
		ImGui::Text("Material properties");
		if (ImGui::InputFloat(u8"Heat capacity [J / (kg * °C)]", materialProperties->getHeatcapacityPointer(), 0, 1000, "%.1f")) {
			selectedShared->updateMaterialProperties();
		}

		if (ImGui::InputFloat("Density [kg/m^3]", materialProperties->getDensityPointer(), 0, 1000, "%.1f")) {
			selectedShared->updateMaterialProperties();
		}
		
		if (ImGui::InputFloat(u8"Thermal conductivity [W / (m * °C)]", materialProperties->getHeatConductivityPointer(), 0, 1000, "%.1f")) {
			selectedShared->updateMaterialProperties();
		}

		if (ImGui::InputFloat(u8"Heat transfer coeff. [W / (m^2 * °C) ]", materialProperties->getHeatTransferCoefficientPointer(), 0, 1000, "%.1f")) {
			selectedShared->updateMaterialProperties();
		}

		ProcessProperties* processProperties = selectedShared->getProcessProperties();
		ImGui::Text("Temperature properties");
		if (ImGui::InputFloat(u8"Initial temperature [°C]", processProperties->getInitialTemperaturePointer(), -50.0f, 100.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
			selectedShared->updateProcessProperties();
		}
		if (ImGui::InputFloat(u8"Environment temperture [°C]", processProperties->getEnvironmentTemperaturePointer(), -50.0f, 100.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
			selectedShared->updateProcessProperties();
		}

		if (*selectedShared->getEndTypePointer() == 0) {
			float* targeTemperature = processProperties->getTargetTemperaturePointer();
			if (ImGui::InputFloat(u8"Target tempreature [°C]", targeTemperature, -50.0f, 100.0f,"%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
				selectedShared->updateProcessProperties();
			};
		}

		ImGui::Text("Time properties");
		if (*selectedShared->getEndTypePointer() == 1) {
			if (ImGui::InputInt("Timespan [min]", processProperties->getTargetTimeSpanPointer(), 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
				selectedShared->updateProcessProperties();
			}
		}

		ImGui::SliderFloat("Timestep [s]", processProperties->getTimeStepPointer(), 0.1f, 5, "%.1f");
		
		int* snapshotInterval = processProperties->getSnapshotIntervalPointer();
		if (ImGui::InputInt("Snapshot interval [s]", snapshotInterval, 5, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
			selectedShared->updateProcessProperties();
		};

		if (currentState > 0) {
			ImGui::EndDisabled();
		}

		Drawing* drawing = selectedShared->getDrawing();
		ImGui::Text("Drawing Properties");
		if (ImGui::SliderFloat("Camera distance [mm]", drawing->getCameraDistancePointer(), 5, 100, "%.1f")) {
			drawing->setCameraDistance(*drawing->getCameraDistancePointer());
		}
		
		if (ImGui::SliderFloat(u8"Camera angle [°]", drawing->getCameraAnglePointer(), 0, 360, "%.1f")) {
			drawing->setCameraAngle(*drawing->getCameraAnglePointer());
		}

		if (ImGui::SliderFloat("Camera height [mm]", drawing->getCameraHeightPointer(), -100, 100, "%.1f")) {
			drawing->setCameraHeight(*drawing->getCameraHeightPointer());
		}

		ImGui::Checkbox("Vertices", drawing->getVerticesSwitchPointer()); ImGui::SameLine();
		ImGui::Checkbox("Lines", drawing->getLineSwitchPointer()); ImGui::SameLine();
		ImGui::Checkbox("Surface", drawing->getSurfaceSwitch());

		if (currentState == 2) {
			ImGui::Text("Results");

			if (*selectedShared->getEndTypePointer() == 0) {
				std::string timeTaken = "Time taken: " + floatToStringWithPrecision(processProperties->getResultTimeSpan(), 0) + " [s]";
				ImGui::Text(timeTaken.c_str());
			}
			else {
				std::string resultTemperature = u8"Final temperature: " + floatToStringWithPrecision(processProperties->getResultTemperature()) + u8" C";
				ImGui::Text(resultTemperature.c_str());
			}
			
			//ImGui::Text("0"); ImGui::SameLine();
			
			ImGui::Image((void*)(intptr_t)selectedShared->getMapTexture(), ImVec2(300, 30), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)); ImGui::SameLine();
			if (ImGui::IsItemHovered())	{
				ImVec2 mousePosition = ImGui::GetMousePos();
				ImVec2 imageStart = ImGui::GetItemRectMin();	
				ImVec2 imageEnd = ImGui::GetItemRectMax();

				float factor = float(mousePosition.x - imageStart.x) / (imageEnd.x - imageStart.x);

				float initialTemperature = *selectedShared->getProcessProperties()->getInitialTemperaturePointer();
				float environmentTemperature = *selectedShared->getProcessProperties()->getEnvironmentTemperaturePointer();

				float temperature = environmentTemperature + (initialTemperature - environmentTemperature) * factor;

				std::string pointedTemperature = "Temperature: " + floatToStringWithPrecision(temperature) + u8"°C";

				ImGui::BeginTooltip();
				ImGui::Text(pointedTemperature.c_str());
				ImGui::EndTooltip();
			}

			ImGui::SameLine();
			ImGui::Text("Heatmap");

			ImGui::NewLine();

			ImGui::PushItemWidth(200);

			if (ImGui::SliderInt("Snapshots", selectedShared->getCurrentSnapshotID(), 0, selectedShared->getSnapshotSize()-1)) {
				selectedShared->setCurrentSnapshot(*selectedShared->getCurrentSnapshotID());
			}

			std::string snapshotTimeText = "Snapshot time: " + floatToStringWithPrecision(selectedShared->getCurrentSnapshotTime(), 0) + " s";
			ImGui::Text(snapshotTimeText.c_str());
			std::string snapshotAverageTempText = u8"Average temperature: " + floatToStringWithPrecision(selectedShared->getCurrentSnapshotAverateTemperature()) + u8" °C";
			ImGui::Text(snapshotAverageTempText.c_str());

			std::string processTimeString = u8"(Calculation time: " + floatToStringWithPrecision(selectedShared->getProcessLength(), 1) + "s)";
			ImGui::Text(processTimeString.c_str());				
		}

		ImGui::PopItemWidth();
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}
