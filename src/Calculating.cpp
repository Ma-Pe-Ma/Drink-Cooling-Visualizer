#include "Calculating.h"
#include <cmath>
#include <iostream>

#include "Snapshot.h"

void Calculating::initialize(int* endType, GeometricProperties* geometricProperties, ProcessProperties* processProperties, MaterialProperties* materialProperties, std::shared_ptr<std::vector<std::shared_ptr<Snapshot>>> snapshots) {
	this->endType = endType;
	this->geometricProperties = geometricProperties;
	this->processProperties = processProperties;
	this->materialProperties = materialProperties;

	this->snapshots = snapshots;

	//std::cout << "Calculating constructed!" << std::endl;
}

void Calculating::update() {
	this->destroy();

	int radiusPointNr = geometricProperties->getRadiusPointNr();
	int axisPointNr = geometricProperties->getAxisPointNr();

	previousRadiusNr = radiusPointNr;

	previousHeatMap = new float* [radiusPointNr];
	currentHeatMap = new float* [radiusPointNr];

	for (int i = 0; i < radiusPointNr; i++) {
		previousHeatMap[i] = new float[axisPointNr];
		currentHeatMap[i] = new float[axisPointNr];

		for (int j = 0; j < axisPointNr; j++) {
			previousHeatMap[i][j] = *processProperties->getInitialTemperaturePointer();
		}
	}

	snapshots->push_back(std::make_shared<Snapshot>(previousHeatMap, *processProperties->getInitialTemperaturePointer(), *processProperties->getInitialTemperaturePointer(), *processProperties->getEnvironmentTemperaturePointer(), 0, geometricProperties));
	(*(snapshots))[0]->generateBuffers();
	//std::cout << "Calculating updated!" << std::endl;
}

bool Calculating::checkFinished() {
	std::future_status status;

	status = finishedFuture.wait_for(std::chrono::milliseconds(0));

	if (status == std::future_status::ready) {
		return finishedFuture.get();
	}

	return false;
}


Calculating::~Calculating() {
	destroy();
}

void Calculating::destroy() {
	if (previousHeatMap != nullptr && currentHeatMap != nullptr) {
		for (int i = 0; i < previousRadiusNr; i++) {
			delete[] previousHeatMap[i];
			delete[] currentHeatMap[i];
		}

		delete[] previousHeatMap;
		delete[] currentHeatMap;

		previousHeatMap = nullptr;
		currentHeatMap = nullptr;
	}

	//std::cout << "Calculating destroyed!" << std::endl;
}

void Calculating::calculate() {
	finishedFuture = finished.get_future();

	// std::cout << "Material properties: " << std::endl;	
	// std::cout << "\tHeat capacity: " << *materialProperties->getHeatcapacityPointer() << std::endl;
	// std::cout << "\tDensity: " << *materialProperties->getDensityPointer()<< std::endl;
	// std::cout << "\tHeat transfer " << *materialProperties->getHeatTransferCoefficientPointer() << std::endl;
	// std::cout << "\tHeat Conductivity: " << *materialProperties->getHeatConductivityPointer() << std::endl;

	// std::cout << "Process properties: " << std::endl;
	// std::cout << "\tEnd Type: " << endType << std::endl;
	// std::cout << "\tTimeStep: " << *processProperties->getTimeStepPointer()<< std::endl;
	// std::cout << "\tSnapshot delta-time: " << *processProperties->getSnapshotIntervalPointer() << std::endl;

	// std::cout << "\tInitial Temperature: " << *processProperties->getInitialTemperaturePointer() << std::endl;
	// std::cout << "\tEnvironment Temperature: " << *processProperties->getEnvironmentTemperaturePointer() << std::endl;

	runProcess.store(true);

	if (*endType == 0) {
		//std::cout << "\tTarget temperature: " << *processProperties->getTargetTemperaturePointer() << std::endl;
		thread = std::thread {&Calculating::calculateTargetTemperature, this};
	}
	else {
		//std::cout << "\tTime span: " << *processProperties->getTargetTimeSpanPointer() << std::endl;
		thread = std::thread{&Calculating::calculateTimeSpan, this};
	}
}

void Calculating::calculateTargetTemperature() {
	unsigned int currentTickNumber = 1;

	float timeStep = *processProperties->getTimeStepPointer(); 
	int snapshotDeltaTime = *processProperties->getSnapshotIntervalPointer();
	float targetTemperature = *processProperties->getTargetTemperaturePointer();

	float lastSnapshotTime = 0;

	while (runProcess.load()) {
		float currentTime = currentTickNumber++ * timeStep;
		unsigned int ellapsedSecs = round(currentTime);

		calculateTimeStep();

		if (currentTime - lastSnapshotTime >= snapshotDeltaTime) {
			float averageTemp = getAverageTemperature(previousHeatMap);
			//std::cout << "averageTemp: " << averageTemp << ", target: " << targetTemperature << std::endl;

			snapshots->push_back(std::make_shared<Snapshot>(previousHeatMap, averageTemp, *processProperties->getInitialTemperaturePointer(), *processProperties->getEnvironmentTemperaturePointer(), ellapsedSecs, geometricProperties));

			if (averageTemp < targetTemperature) {
				processProperties->setResultTimeSpan(ellapsedSecs);
				break;
			}

			lastSnapshotTime = currentTime;
		}
	}

	//std::cout << "snapshot count: " << snapshots->size();

	finished.set_value(true);
}

void Calculating::calculateTimeSpan() {
	//std::cout << "Calculating time span!" << std::endl;

	float timeStep = *processProperties->getTimeStepPointer();
	float targetTimeSpan = *processProperties->getTargetTimeSpanPointer() * 60;
	int snapshotDeltaTime = *processProperties->getSnapshotIntervalPointer();

	unsigned int timeticks = targetTimeSpan / timeStep;

	float lastSnapshotTime = 0;

	//std::cout << "Time ticks: " << timeticks << std::endl;
	
	for (int t = 1; t < (timeticks + 1) && runProcess.load(); t++) {
		float currentTime = t * timeStep;

		unsigned int ellapsedSecs = round(currentTime);

		calculateTimeStep();

		if (currentTime - lastSnapshotTime >= snapshotDeltaTime) {
			//std::cout << "currentTime: " << currentTime << ", ellapsedSecs: " << ellapsedSecs << std::endl;
			float averageTemp = getAverageTemperature(previousHeatMap);

			snapshots->push_back(std::make_shared<Snapshot>(previousHeatMap, averageTemp, *processProperties->getInitialTemperaturePointer(), *processProperties->getEnvironmentTemperaturePointer(), ellapsedSecs, geometricProperties));

			if (currentTime >= targetTimeSpan) {
				processProperties->setResultTemperature(averageTemp);
				break;
			}

			lastSnapshotTime = currentTime;
		}
	}

	//std::cout << "Snapshot count: " << snapshots->size();

	finished.set_value(true);
}

void Calculating::stop() {
	if (runProcess.load()) {
		runProcess.store(false);
		thread.join();
	}
}

void Calculating::calculateTimeStep() {
	float transferConstant = materialProperties->getTransferConstant();
	float conductionConstant = materialProperties->getConductionConstant();
	float environmentTemperature = *processProperties->getEnvironmentTemperaturePointer();

	float* radiusSectionLengths = geometricProperties->getRadiusSectionsPointer();
	float* axisSectionLengths = geometricProperties->getAxisSectionsPointer();

	float* radiusPoints = geometricProperties->getRadiusPointsPointer();
	float* axisPoints = geometricProperties->getAxisPointsPointer();

	int axisPointNr = geometricProperties->getAxisPointNr();
	int radiusPointNr = geometricProperties->getRadiusPointNr();

	float timeStep = *processProperties->getTimeStepPointer();

	for (int i = 0; i < radiusPointNr; i++) {
		for (int j = 0; j < axisPointNr; j++) {
			float radComp;

			//transfer at the side of the cylinder
			if (i == (radiusPointNr - 1)) {
				radComp = -transferConstant * (previousHeatMap[i][j] - environmentTemperature) / radiusSectionLengths[i] * timeStep;
			}
			//conduction inside the material
			else {
				float temperatureDifferenceBydr = (previousHeatMap[i + 1][j] - previousHeatMap[i][j]) / radiusSectionLengths[i];
				radComp = conductionConstant * temperatureDifferenceBydr / (radiusPoints[i] + radiusSectionLengths[i]) * timeStep;
			}

			//calculating bottom part
			if (j < axisPointNr / 2 + 1) {
				float axisComp;

				//transfer at the bottom of the cylinder
				if (j == 0) {
					axisComp = -transferConstant * (previousHeatMap[i][j] - environmentTemperature) / axisSectionLengths[0] * timeStep;
				}
				//conduction inside the material
				else {
					axisComp = conductionConstant * (previousHeatMap[i][j - 1] - previousHeatMap[i][j]) / (axisSectionLengths[j] * axisSectionLengths[j] ) * timeStep;

					//middle section gets heat from both directions
					if (j == axisPointNr / 2) {
						axisComp *= 2;
					}
				}

				currentHeatMap[i][j] = previousHeatMap[i][j] + radComp + axisComp;
			}
			//symmetrical top part
			else {
				currentHeatMap[i][j] = currentHeatMap[i][axisPointNr - 1 - j];
			}
		}
	}

	float** temp = previousHeatMap;
	previousHeatMap = currentHeatMap;
	currentHeatMap = temp;
}

float Calculating::getAverageTemperature(float** heatMap) {
	float sumWeigthedTemp = 0;

	int radiusPointNr = geometricProperties->getRadiusPointNr();
	int axisPointNr = geometricProperties->getAxisPointNr();

	float* radiusPoints = geometricProperties->getRadiusPointsPointer();
	float* axisPoints = geometricProperties->getAxisPointsPointer();

	for (int i = 0; i < radiusPointNr - 1; i++) {
		for (int j = 0; j < axisPointNr - 1; j++) {
			float avTemp = (heatMap[i][j] + heatMap[i][j + 1] + heatMap[i + 1][j] + heatMap[i + 1][j + 1]) / 4;
			float body = (PI * (radiusPoints[i + 1] * radiusPoints[i + 1] - radiusPoints[i] * radiusPoints[i]) * (axisPoints[j + 1] - axisPoints[j]));

			sumWeigthedTemp += avTemp * body;
		}
	}

	return sumWeigthedTemp / geometricProperties->getVolume();
}