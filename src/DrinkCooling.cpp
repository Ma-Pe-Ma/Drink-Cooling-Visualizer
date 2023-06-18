#include "DrinkCooling.h"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include <string>

DrinkCooling::DrinkCooling(int idCounter) {
	snapshots = std::make_shared<std::vector<std::shared_ptr<Snapshot>>>();
	drawing = std::make_shared<Drawing>();

	calculating.initialize(&endType, &geometricProperties, &processProperties, &materialProperties, snapshots);

	this->name = std::to_string(++idCounter) + ". process";

	this->geometricProperties.update();
	this->processProperties.update();
	this->materialProperties.update();

	drawing->setGeometricProperties(&geometricProperties);

	calculating.update();
	drawing->update();

	current = (*snapshots)[0];
}

void DrinkCooling::updateGeometry() {
	snapshots->clear();

	geometricProperties.update();	
	calculating.update();
	drawing->update();

	current = (*snapshots)[0];

	//std::cout << "Geometry properties updated!" << std::endl;
}

void DrinkCooling::updateProcessProperties() {
	snapshots->clear();

	processProperties.update();
	calculating.update();

	current = (*snapshots)[0];
	//std::cout << "Process properties updated!" << std::endl;
}

void DrinkCooling::updateMaterialProperties() {
	materialProperties.update();
	//std::cout << "Material properties updated!" << std::endl;
}

void DrinkCooling::launchProcess() {
	state = 1;

	*geometricProperties.getAxisNrGUI() += *geometricProperties.getAxisNrGUI() % 2;
	updateGeometry();

	processStartTime = std::chrono::system_clock::now();

	calculating.calculate();
}

bool DrinkCooling::checkProcessFinish() {
	if (calculating.checkFinished()) {
		state = 2;		

		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - processStartTime;
		processLength = work_time.count() / 1000;

		snapshotTimes = new float[snapshots->size()];
		snapshotAverageTemperatures = new float[snapshots->size()];
		snapshotTimes[0] = (*snapshots)[0]->getTime();
		snapshotAverageTemperatures[0] = (*snapshots)[0]->getAverageTemperature();

		for (auto snapshot = ++snapshots->begin(); snapshot != snapshots->end(); ++snapshot) {
			(*snapshot)->generateBuffers();

			int index = snapshot - snapshots->begin();

			snapshotTimes[index] = (*snapshot)->getTime();
			snapshotAverageTemperatures[index] = (*snapshot)->getAverageTemperature();

			//std::cout << "SnapshotTime: " << snapshotTimes[index] << std::endl;
			//std::cout << "AVT: " << snapshotAverageTemperatures[index] << std::endl;
		}

		return true;
	}

	return false;
}

std::string DrinkCooling::getCurrentStateString(int state) {
	switch (state) {
	case 0:
		return "configuration";
	case 1:
		return "calculating...";
	case 2:
		return "post-process";
	}

	return "unknown";
}

DrinkCooling::~DrinkCooling() {
	if (snapshotTimes != nullptr)
	{
		delete[] snapshotTimes;
		snapshotTimes = nullptr;
	}
	
	if (snapshotAverageTemperatures != nullptr)
	{
		delete[] snapshotAverageTemperatures;
		snapshotAverageTemperatures = nullptr;
	}	
}