#include "DrinkCooling.h"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include <string>

unsigned int DrinkCooling::idCounter = 0;


DrinkCooling::DrinkCooling() {
	calculating.initialize(&endType, &geometricProperties, &processProperties, &materialProperties, &snapshots);

	this->name = std::to_string(++DrinkCooling::idCounter) + ". process";

	this->geometricProperties.update();
	this->processProperties.update();
	this->materialProperties.update();

	drawing.setGeometricProperties(&geometricProperties);

	calculating.update();
	drawing.update();

	current = snapshots[0];
	current.lock()->generateBuffers();
}

void DrinkCooling::updateGeometry() {
	calculating.destroy();
	geometricProperties.update();
	calculating.update();
	current = snapshots[0];
	current.lock()->generateBuffers();
	drawing.update();
	drawing.determineCameraMatrix();
	//std::cout << "Geometry properties updated!" << std::endl;
}

void DrinkCooling::updateProcessProperties() {
	processProperties.update();
	calculating.destroy();
	calculating.update();
	current = snapshots[0];
	current.lock()->generateBuffers();
	//std::cout << "Process properties updated!" << std::endl;
}

void DrinkCooling::updateMaterialProperties() {
	materialProperties.update();
	//std::cout << "Material properties updated!" << std::endl;
}

void DrinkCooling::visualize() {
	drawing.visualize(current.lock());
}

void DrinkCooling::launchProcess() {
	state = 1;

	*geometricProperties.getAxisNrGUI() += *geometricProperties.getAxisNrGUI() % 2;
	updateGeometry();

	processStartTime = std::chrono::system_clock::now();

	calculating.calculate();
	drawing.initializeMap();
}

void DrinkCooling::checkProcesFinish() {
	if (calculating.checkFinished()) {
		state = 2;		

		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - processStartTime;
		processLength = work_time.count() / 1000;

		for (auto snapshot = ++snapshots.begin(); snapshot != snapshots.end(); ++snapshot) {
			(*snapshot)->generateBuffers();
		}
	}
}

DrinkCooling::~DrinkCooling() {

}