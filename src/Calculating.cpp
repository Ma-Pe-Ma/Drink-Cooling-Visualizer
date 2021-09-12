#include "Calculating.h"
#include <cmath>
#include <iostream>

void Calculating::initialize(int* endType, GeometricProperties* geometricProperties, ProcessProperties* processProperties, MaterialProperties* materialProperties, std::vector<std::shared_ptr<Snapshot>>* snapshots) {
	this->endType = endType;
	this->geometricProperties = geometricProperties;
	this->processProperties = processProperties;
	this->materialProperties = materialProperties;

	this->snapshots = snapshots;

	std::cout << "Calculating constructed!" << std::endl;
}

void Calculating::update() {
	snapshots->clear();

	this->geometricProperties = geometricProperties;
	this->processProperties = processProperties;

	int radiusPointNr = geometricProperties->getRadiusPointNr();
	int axisPointNr = geometricProperties->getAxisPointNr();

	previousHeatMap = new float* [radiusPointNr];
	currentHeatMap = new float* [radiusPointNr];

	for (int i = 0; i < radiusPointNr; i++) {
		previousHeatMap[i] = new float[axisPointNr];
		currentHeatMap[i] = new float[axisPointNr];

		for (int j = 0; j < axisPointNr; j++) {
			if (i == radiusPointNr - 1 || j == axisPointNr - 1 || j == 0) {
				previousHeatMap[i][j] = *processProperties->getEnvironmentTemperaturePointer();
			}
			else {
				previousHeatMap[i][j] = *processProperties->getInitialTemperaturePointer();
			}
			currentHeatMap[i][j] = 0;
		}
	}

	snapshots->push_back(std::unique_ptr<Snapshot>(new Snapshot(previousHeatMap, *processProperties->getInitialTemperaturePointer(), *processProperties->getInitialTemperaturePointer(), *processProperties->getEnvironmentTemperaturePointer(), 0, geometricProperties)));
	std::cout << "Calculating updated!" << std::endl;
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
	thread.detach();

	destroy();
}

void Calculating::destroy() {
	if (previousHeatMap != nullptr && currentHeatMap != nullptr) {
		for (int i = 0; i < geometricProperties->getRadiusPointNr(); i++) {
			delete[] previousHeatMap[i];
			delete[] currentHeatMap[i];
		}

		delete[] previousHeatMap;
		delete[] currentHeatMap;
	}

	std::cout << "Calculating destroyed!" << std::endl;
}

void Calculating::calculate() {
	finishedFuture = finished.get_future();

	std::cout << "Material properties: " << std::endl;	
	std::cout << "\tHeat capacity: " << *materialProperties->getHeatcapacityPointer() << std::endl;
	std::cout << "\tDensity: " << *materialProperties->getDensityPointer()<< std::endl;
	std::cout << "\tHeat transfer " << *materialProperties->getHeatTransferCoefficientPointer() << std::endl;
	std::cout << "\tHeat Conductivity: " << *materialProperties->getHeatConductivityPointer() << std::endl;

	std::cout << "Process properties: " << std::endl;
	std::cout << "\tEnd Type: " << endType << std::endl;
	std::cout << "\tTimeStep: " << *processProperties->getTimeStepPointer()<< std::endl;
	std::cout << "\tSnapshot delta-time: " << *processProperties->getSnapshotIntervalPointer() << std::endl;

	std::cout << "\tInitial Temperature: " << *processProperties->getInitialTemperaturePointer() << std::endl;
	std::cout << "\tEnvironment Temperature: " << *processProperties->getEnvironmentTemperaturePointer() << std::endl;


	if (*endType == 0) {
		std::cout << "\tTarge temperature: " << *processProperties->getTargetTemperaturePointer() << std::endl;
		thread = std::thread {&Calculating::calculateTargetTemperature, this};
		//thread.detach();
	}
	else {
		std::cout << "\tTimeSpan: " << *processProperties->getTargetTimeSpanPointer() << std::endl;
		thread = std::thread{&Calculating::calculateTimeSpan, this};
		//thread.detach();
	}
}

void Calculating::calculateTargetTemperature() {
	unsigned int currentTickNumber = 1;

	float timeStep = *processProperties->getTimeStepPointer(); 
	int snapshotDeltaTime = *processProperties->getSnapshotIntervalPointer();
	float targetTemperature = *processProperties->getTargetTemperaturePointer();

	float lastSnapshotTime = 0;

	while (true) {
		float currentTime = currentTickNumber++ * timeStep;
		unsigned int ellapsedSecs = round(currentTime);

		calculateTimeStep(false);

		if (currentTime - lastSnapshotTime >= snapshotDeltaTime) {
			float averageTemp = getAverageTemperature(previousHeatMap);
			std::cout << "averageTemp: " << averageTemp << ", target: " << targetTemperature << std::endl;

			snapshots->push_back(std::unique_ptr<Snapshot>(new Snapshot(previousHeatMap, averageTemp, *processProperties->getInitialTemperaturePointer(), *processProperties->getEnvironmentTemperaturePointer(), ellapsedSecs, geometricProperties)));

			if (averageTemp < targetTemperature) {
				processProperties->setResultTimeSpan(ellapsedSecs);
				break;
			}

			lastSnapshotTime = currentTime;
		}
	}

	std::cout << "snapshot count: " << snapshots->size();

	finished.set_value(true);
}

void Calculating::calculateTimeSpan() {
	std::cout << "Calculating time span!" << std::endl;

	float timeStep = *processProperties->getTimeStepPointer();
	float targetTimeSpan = *processProperties->getTargetTimeSpanPointer() * 60;
	int snapshotDeltaTime = *processProperties->getSnapshotIntervalPointer();

	unsigned int timeticks = targetTimeSpan / timeStep;

	float lastSnapshotTime = 0;

	std::cout << "timeticks: " << timeticks << std::endl;

	for (int t = 1; t < timeticks + 1; t++) {
		float currentTime = t * timeStep;

		unsigned int ellapsedSecs = round(currentTime);

		calculateTimeStep(false);

		if (currentTime - lastSnapshotTime >= snapshotDeltaTime) {
			std::cout << "currentTime: " << currentTime << ", ellapsedSecs: " << ellapsedSecs << std::endl;
			float averageTemp = getAverageTemperature(previousHeatMap);

			snapshots->push_back(std::unique_ptr<Snapshot>(new Snapshot(previousHeatMap, averageTemp, *processProperties->getInitialTemperaturePointer(), *processProperties->getEnvironmentTemperaturePointer(), ellapsedSecs, geometricProperties)));

			if (currentTime >= targetTimeSpan) {
				processProperties->setResultTemperature(averageTemp);
				break;
			}

			lastSnapshotTime = currentTime;
		}
	}

	std::cout << "snapshot count: " << snapshots->size();

	finished.set_value(true);
}

void Calculating::calculateTimeStep(bool debug) {
	float max = 0;
	float min = 600;

	int maxi = -1;
	int maxj = -1;

	int mini = -1;
	int minj = -1;

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

			if (i == radiusPointNr - 1 || j == 0 || j == geometricProperties->getAxisPointNr() - 1) {
				currentHeatMap[i][j] = environmentTemperature;
			}
			else {
				float radComp;

				//unhandled r=0 case :/
				if (i == 0) {
					radComp = 0;
				}
				//transfer at the edge of can (at the mantle)
				else if (i == radiusPointNr - 2) {
					radComp = -transferConstant * (previousHeatMap[i][j] - environmentTemperature) / radiusSectionLengths[i] * timeStep;
				}
				//conduction inside the material
				else {
					float temperatureDifferenceBydr = (previousHeatMap[i + 1][j] - previousHeatMap[i][j]) / radiusSectionLengths[i];
					radComp = conductionConstant * temperatureDifferenceBydr / (radiusPoints[i] + radiusSectionLengths[i]) * timeStep;
				}

				float axisComp;

				// transfer at bottom of the cylinder
				if (j == 1) {
					axisComp = -transferConstant * (previousHeatMap[i][j] - environmentTemperature) / axisSectionLengths[0] * timeStep;
				}
				// transfer at top of the cylinder
				else if (j == axisPointNr - 2) {
					axisComp = -transferConstant * (previousHeatMap[i][j] - environmentTemperature) / axisSectionLengths[axisPointNr - 2] * timeStep;
				}
				// conduction inside the material
				else {
					float temperatureDifferenceBydz;

					if (j > (axisPointNr - 1) / 2) {
						temperatureDifferenceBydz = (previousHeatMap[i][j + 1] - previousHeatMap[i][j]) / axisSectionLengths[j];
					}
					else {
						temperatureDifferenceBydz = (previousHeatMap[i][j - 1] - previousHeatMap[i][j]) / axisSectionLengths[j];
					}

					axisComp = conductionConstant * temperatureDifferenceBydz / axisSectionLengths[j] * timeStep;
				}

				currentHeatMap[i][j] = previousHeatMap[i][j] + radComp + axisComp;

				if (currentHeatMap[i][j] > max) {
					max = currentHeatMap[i][j];
					maxi = i;
					maxj = j;
				}

				if (currentHeatMap[i][j] < min) {
					min = currentHeatMap[i][j];
					mini = i;
					minj = j;
				}

				/*if (i ==10 && j == 10 && previousHeatMap[i][j] > 12 && previousHeatMap[i][j] < 13) {
					std::cout << "radcomp: " << radComp << std::endl;
					std::cout << "axiscomp: " << axisComp << std::endl;
					std::cout << "axis[0][20] " << currentHeatMap[0][20] << std::endl;
				}

				if (i == 0 && j == 1) {
					static unsigned int valami = 0;
					if (++valami < 20) {
						std::cout << "rad: " << radComp << ", ax: " << axisComp << ", prev: " << previousHeatMap[i][j] << std::endl;

						std::cout << "axisdelta[0]: " << axisSectionLengths[0] << std::endl;
					}
				}*/
			}
		}
	}

	/*if (debug) {
		std::cout << "max: " << max << ", i: " << maxi << ", j: " << maxj << std::endl;
		std::cout << "min: " << min << ", i: " << mini << ", j: " << minj << std::endl;

		for (int i = 0; i < axisPointNr; i++) {
			std::cout << "tengely " << i << ": " << currentHeatMap[30][i] << std::endl;
		}
	}*/

	float** temp = previousHeatMap;
	previousHeatMap = currentHeatMap;
	currentHeatMap = temp;
}

float Calculating::getAverageTemperature(float** heatMap) {
	float sumWeigthedTemp = 0;

	bool once = true;

	int check = 15;

	int higherThanCheck = 0;
	int lowerThanCheck = 0;

	int radiusPointNr = geometricProperties->getRadiusPointNr();
	int axisPointNr = geometricProperties->getAxisPointNr();

	float* radiusPoints = geometricProperties->getRadiusPointsPointer();
	float* axisPoints = geometricProperties->getAxisPointsPointer();

	for (int i = 0; i < radiusPointNr - 1; i++) {
		for (int j = 0; j < axisPointNr - 1; j++) {
			float avTemp = (heatMap[i][j] + heatMap[i][j + 1] + heatMap[i + 1][j] + heatMap[i + 1][j + 1]) / 4;
			float body = (PI * (radiusPoints[i + 1] * radiusPoints[i + 1] - radiusPoints[i] * radiusPoints[i]) * (axisPoints[j + 1] - axisPoints[j]));

			if (avTemp > check) {
				higherThanCheck++;
			}
			else {
				lowerThanCheck++;
			}

			sumWeigthedTemp += avTemp * body;
		}
	}

	//std::cout << "sum weight temp: " << sumWeigthedTemp << std::endl;

	float averageTemp = sumWeigthedTemp / geometricProperties->getVolume();
	//std::cout << "Average temp: " << averageTemp << std::endl;
	//std::cout << "higher than "<<check <<": " << higherThanCheck << "lower than 15: " << lowerThanCheck << std::endl;	

	return averageTemp;
}