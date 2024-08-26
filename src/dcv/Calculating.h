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
	float** previousHeatMap = nullptr;
	float** currentHeatMap = nullptr;

	GeometricProperties* geometricProperties;
	ProcessProperties* processProperties;
	MaterialProperties* materialProperties;

	int* endType;

	void calculateTimeSpan();
	void calculateTargetTemperature();
	void calculateTimeStep();
	float getAverageTemperature(float**);

	std::shared_ptr<std::vector<std::shared_ptr<Snapshot>>> snapshots;

	std::promise<bool> finished;
	std::future<bool> finishedFuture;
	std::thread thread;
	std::atomic<bool> runProcess;

	int previousRadiusNr = 0;
public:
	//Calculating(int*, GeometricProperties*, ProcessProperties*, MaterialProperties*, std::vector<std::shared_ptr<Snapshot>>*);
	Calculating() {}
	Calculating(const Calculating&) = delete;
	~Calculating();

	Calculating& operator= (const Calculating&) = delete;

	void initialize(int* endType, GeometricProperties* geometricProperties, ProcessProperties* processProperties, MaterialProperties* materialProperties, std::shared_ptr<std::vector<std::shared_ptr<Snapshot>>> snapshots);
	
	void update();
	void destroy();	

	void calculate();
	bool checkFinished();
	void stop();
};

#endif