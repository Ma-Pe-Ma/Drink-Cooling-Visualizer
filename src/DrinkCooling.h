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
	//id properties
	static unsigned int idCounter;
	std::string name;

	//state 0 - uninitialized, 1 - calculating, 2 - postprocess
	int state = 0;

	//enum for end type (int used because of imgui) 0 - targettemperature, 1 - timespan
	int endType = 0;

	GeometricProperties geometricProperties = GeometricProperties(RADIUS_LENGTH, AXIS_LENGTH, RADIUS_SECTION_NR, AXIS_SECTION_NR, SECTION_ANGLE);
	ProcessProperties processProperties = ProcessProperties(INITIAL_TEMPERATURE, ENVIRONMENT_TEMPERATURE, TARGET_TIME_SPAN, SNAPSHOT_DELTA_TIME, TIME_STEP);
	MaterialProperties materialProperties = MaterialProperties(HEAT_CAPACITY, DENSITY, HEAT_CONDUCTIVITY, HEAT_TRANSFER_COEFFICIENT);
	
	//Calculating calculating = Calculating(&endType, &geometricProperties, &processProperties, &materialProperties, &snapshots);	
	Calculating calculating;

	Drawing drawing;

	//result containers
	std::vector<std::shared_ptr<Snapshot>> snapshots;
	std::weak_ptr<Snapshot> current;

	unsigned int mapTextureId;

public:
	DrinkCooling();
	~DrinkCooling();

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

	std::string getCurrentStateString(int state) {
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

	int* getEndTypePointer() {
		return &endType;
	}

	void updateGeometry();
	void updateMaterialProperties();
	void updateProcessProperties();

	GeometricProperties* getGeometricProperties() {
		return &geometricProperties;
	}

	MaterialProperties* getMaterialProperties() {
		return &materialProperties;
	}

	ProcessProperties* getProcessProperties() {
		return &processProperties;
	}

	Drawing* getDrawing() {
		return &drawing;
	}

	void visualize();
	void launchProcess();
	void checkProcesFinish();

	int getSnapshotSize() {
		return snapshots.size();
	}

	void setCurrentSnapshot(int currentID) {
		current = snapshots[currentID];
	}

	float getCurrentSnapshotAverateTemperature() {
		return current.lock()->getAverageTemperature();
	}

	int getCurrentSnapshotTime() {
		return current.lock()->getTime();
	}

	unsigned int getMapTexture() {
		return drawing.getTexure();
	}
};

#endif