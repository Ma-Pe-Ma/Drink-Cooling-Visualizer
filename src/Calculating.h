#ifndef CALCULATING_H
#define CALCULATING_H

#include "GeometricProperties.h"
#include "ProcessProperties.h"
#include "MaterialProperties.h"

#include "Snapshot.h"

#include <memory>
#include <vector>

#include <future>
#include <thread>

class Calculating {
	float** previousHeatMap;
	float** currentHeatMap;

	GeometricProperties* geometricProperties;
	ProcessProperties* processProperties;
	MaterialProperties* materialProperties;

	int* endType;

	void calculateTimeSpan();
	void calculateTargetTemperature();
	void calculateTimeStep(bool);
	float getAverageTemperature(float**);

	std::vector<std::shared_ptr<Snapshot>>* snapshots;

	std::promise<bool> finished;
	std::future<bool> finishedFuture;
	std::thread thread;
	std::atomic<bool> runProcess;

public:
	//Calculating(int*, GeometricProperties*, ProcessProperties*, MaterialProperties*, std::vector<std::shared_ptr<Snapshot>>*);
	Calculating() {}
	Calculating(const Calculating&) = delete;
	Calculating& operator= (const Calculating&) = delete;
	void destroy();
	void calculate();
	void update();
	void initialize(int* endType, GeometricProperties* geometricProperties, ProcessProperties* processProperties, MaterialProperties* materialProperties, std::vector<std::shared_ptr<Snapshot>>* snapshots);
	~Calculating();

	bool checkFinished();
	void stop();
};

#endif