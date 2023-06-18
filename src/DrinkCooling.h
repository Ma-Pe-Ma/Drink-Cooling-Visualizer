#ifndef DRINK_COOLING
#define DRINK_COOLING

#include <vector>

#define PI 3.14159265359f

#include <memory>

#include "GeometricProperties.h"
#include "MaterialProperties.h"
#include "ProcessProperties.h"

#include "Calculating.h"

#include "Drawing.h"

#include "Snapshot.h"

#define RADIUS_SECTION_NR 32
#define AXIS_SECTION_NR 160

#define RADIUS_LENGTH 32
#define AXIS_LENGTH 160

#define SECTION_ANGLE 45

#define INITIAL_TEMPERATURE 24
#define ENVIRONMENT_TEMPERATURE 4

#define TIME_STEP 0.1
#define TARGET_TIME_SPAN 60
#define SNAPSHOT_DELTA_TIME 5

#define HEAT_CAPACITY 4184 
#define DENSITY 1000
#define HEAT_CONDUCTIVITY 0.5918
#define HEAT_TRANSFER_COEFFICIENT 5.0

class DrinkCooling {
private:
	std::string name;

	//enum: state 0 - uninitialized, 1 - calculating, 2 - postprocess
	int state = 0;

	//enum for end type (int used because of imgui) 0 - targettemperature, 1 - timespan
	int endType = 0;

	GeometricProperties geometricProperties = GeometricProperties(RADIUS_LENGTH, AXIS_LENGTH, RADIUS_SECTION_NR, AXIS_SECTION_NR, SECTION_ANGLE);
	ProcessProperties processProperties = ProcessProperties(INITIAL_TEMPERATURE, ENVIRONMENT_TEMPERATURE, TARGET_TIME_SPAN, SNAPSHOT_DELTA_TIME, TIME_STEP);
	MaterialProperties materialProperties = MaterialProperties(HEAT_CAPACITY, DENSITY, HEAT_CONDUCTIVITY, HEAT_TRANSFER_COEFFICIENT);
	
	//Calculating calculating = Calculating(&endType, &geometricProperties, &processProperties, &materialProperties, &snapshots);	
	Calculating calculating;

	std::shared_ptr<Drawing> drawing;

	//result containers
	std::shared_ptr<std::vector<std::shared_ptr<Snapshot>>> snapshots;
	std::weak_ptr<Snapshot> current;
	int currentSnapshotId = 0;

	std::chrono::system_clock::time_point processStartTime;
	float processLength = 0;	

	float* snapshotTimes = nullptr;
	float* snapshotAverageTemperatures = nullptr;

public:
	DrinkCooling(int);
	~DrinkCooling();

	void updateGeometry();
	void updateMaterialProperties();
	void updateProcessProperties();

	void launchProcess();
	bool checkProcessFinish();

	std::string getCurrentStateString(int state);

	void setEndType(int endtype) { this->endType = endtype; }

	std::string getName() {
		return name;
	}

	void setName(std::string name) {
		this->name = name;
	}

	int getCurrentState() {
		return state;
	}	

	int* getEndTypePointer() {
		return &endType;
	}
	
	GeometricProperties* getGeometricProperties() {
		return &geometricProperties;
	}

	MaterialProperties* getMaterialProperties() {
		return &materialProperties;
	}

	ProcessProperties* getProcessProperties() {
		return &processProperties;
	}

	std::shared_ptr<Drawing> getDrawing() {
		return drawing;
	}

	int getSnapshotSize() {
		return snapshots->size();
	}

	void setCurrentSnapshot(int currentID) {
		current = (*snapshots)[currentID];
	}

	std::weak_ptr<Snapshot> getCurrentSnapshot()
	{
		return this->current;
	}

	float getCurrentSnapshotAverateTemperature() {
		return current.lock()->getAverageTemperature();
	}

	int getCurrentSnapshotTime() {
		return current.lock()->getTime();
	}

	void stop() {
		calculating.stop();
	}

	float getProcessLength() {
		return processLength;
	}

	int* getCurrentSnapshotID() {
		return &currentSnapshotId;
	}

	float* getSnapshotTimes()
	{
		return snapshotTimes;
	}

	float* getSnapshotAverageTemperatures()
	{
		return snapshotAverageTemperatures;
	}
};

#endif