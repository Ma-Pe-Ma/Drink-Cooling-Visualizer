#include "MainLogic.h"

#include <sstream>
#include <iomanip>

#include "Windowing.h"
#include "Drawer.h"

std::string floatToStringWithPrecision(float number, int precision = 2) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << number;
	return stream.str();
}

void MainLogic::initialize() {
	drawer = std::make_shared<Drawer>();
	drawer->initialize(windowing->getImguiWidth());
	drawer->updateProjectionMatrix(windowing->getScreenWidth() - windowing->getImguiWidth(), windowing->getScreenHeight());

	drinkCoolings.push_back(std::make_shared<DrinkCooling>(0));
	selected = drinkCoolings[0];
	selected.lock()->getDrawing()->determineCameraMatrix();

	windowing->updateViewPort();
	windowSize = ImVec2(400, windowing->getScreenHeight());
}

void MainLogic::update() {
	auto selectedShared = selected.lock();

	if (selectedShared.get()) {
		drawer->visualizeCooling(selectedShared);
	}

	ImGui::SetNextWindowPos(ImVec2(0,0));
	ImGui::SetNextWindowSize(ImVec2(windowing->getImguiWidth(), windowing->getScreenHeight()));

	ImGui::Begin("Drink Cooling Visualizer", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginListBox(u8"Processes", ImVec2(0, 60))) {
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
		drinkCoolings.push_back(std::make_shared<DrinkCooling>(drinkCoolings.size()));
	}

	ImGui::NewLine();

	if (selectedShared.get() != nullptr) {
		ImGui::PushItemWidth(100);

		int currentState = selectedShared->getCurrentState();

		auto selectedName = "Selected: " + selectedShared->getName();
		ImGui::Text(selectedName.c_str());

		std::string currentStateString = "Current state: " + selectedShared->getCurrentStateString(currentState);
		ImGui::Text(currentStateString.c_str());
		if (currentState == 0) {
			ImGui::SameLine();
			if (ImGui::Button("Start")) {
				selectedShared->launchProcess();
				windowing->updateViewPort();
			}
		}
		else if (currentState == 1) {
			if (selectedShared->checkProcessFinish())
			{
				collapseConfiguration = true;
			}
		}

		if (currentState > 0) {
			ImGui::BeginDisabled();
		}

		ImGui::Text("Ending condition");
		ImGui::RadioButton("Target temperature", selectedShared->getEndTypePointer(), 0); ImGui::SameLine();
		ImGui::RadioButton("Timespan", selectedShared->getEndTypePointer(), 1);

		if (currentState > 0) {
			ImGui::EndDisabled();
		}

		if (collapseConfiguration) {
			ImGui::SetNextItemOpen(false, ImGuiTreeNodeFlags_DefaultOpen);
		}

		if (ImGui::CollapsingHeader("Configuration", ImGuiTreeNodeFlags_DefaultOpen) ) {
			if (currentState > 0) {
				ImGui::BeginDisabled();
			}

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

			if (ImGui::InputFloat(u8"Heat transfer coeff. [W / (m^2 * °C)]", materialProperties->getHeatTransferCoefficientPointer(), 0, 1000, "%.1f")) {
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
				if (ImGui::InputFloat(u8"Target tempreature [°C]", targeTemperature, -50.0f, 100.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
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
		}

		std::shared_ptr<Drawing> drawing = selectedShared->getDrawing();
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

		ImGui::Checkbox("Vertices", drawing->getVertexSwitchPointer()); ImGui::SameLine();
		ImGui::Checkbox("Lines", drawing->getLineSwitchPointer()); ImGui::SameLine();
		ImGui::Checkbox("Surface", drawing->getSurfaceSwitchPointer());

		if (currentState == 2) {
			ImGui::NewLine();
			ImGui::Text("Results:");

			ProcessProperties* processProperties = selectedShared->getProcessProperties();

			if (*selectedShared->getEndTypePointer() == 0) {
				std::string timeTaken = "Time taken: " + floatToStringWithPrecision(processProperties->getResultTimeSpan(), 0) + " [s]";
				ImGui::Text(timeTaken.c_str());
			}
			else {
				std::string resultTemperature = u8"Final temperature: " + floatToStringWithPrecision(processProperties->getResultTemperature()) + u8" °C";
				ImGui::Text(resultTemperature.c_str());
			}

			const int colorMapWidth = windowing->getImguiWidth() * 3 / 4;

			ImGui::Image((void*)(intptr_t) drawer->getHeatMapTextureColorBuffer(), ImVec2(colorMapWidth, colorMapWidth / 10), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
			ImGui::SameLine();
			if (ImGui::IsItemHovered()) {
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

			if (ImGui::SliderInt("Snapshots", selectedShared->getCurrentSnapshotID(), 0, selectedShared->getSnapshotSize() - 1)) {
				selectedShared->setCurrentSnapshot(*selectedShared->getCurrentSnapshotID());
			}

			std::string snapshotTimeText = "Snapshot time: " + floatToStringWithPrecision(selectedShared->getCurrentSnapshotTime(), 0) + " s";
			ImGui::Text(snapshotTimeText.c_str());
			std::string snapshotAverageTempText = u8"Average temperature: " + floatToStringWithPrecision(selectedShared->getCurrentSnapshotAverateTemperature()) + u8" °C";
			ImGui::Text(snapshotAverageTempText.c_str());

			std::string processTimeString = u8"(Calculation time: " + floatToStringWithPrecision(selectedShared->getProcessLength(), 1) + "s)";
			ImGui::Text(processTimeString.c_str());

			if (ImPlot::BeginPlot("Average temparature by time")) {
				ImPlot::SetupLegend(ImPlotLocation_North | ImPlotLocation_East, ImPlotLegendFlags_Horizontal);
				ImPlot::SetupAxes("Time [s]", u8"Temperature [°C]");
				ImPlot::PlotLine("T(t)", selectedShared->getSnapshotTimes(), selectedShared->getSnapshotAverageTemperatures(), selectedShared->getSnapshotSize());
				ImPlot::EndPlot();
			}
		}

		ImGui::PopItemWidth();
	}

	ImGui::End();
}

void MainLogic::terminate() {
	for (auto it = drinkCoolings.begin(); it != drinkCoolings.end(); it++) {
		(*it)->stop();
	}
}