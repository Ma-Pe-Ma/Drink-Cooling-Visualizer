#ifndef PROCESSPROPERTIES_H
#define PROCESSPROPERTIES_H

class ProcessProperties {
	//temperature boundary conditions
	float initialTemperature;
	float environmentTemperature;
	float targetTemperature;
	
	//time properties
	int targetTimeSpan;
	int snapshotDeltaTime;
	float timeStep;	

	//result containers
	float resultTemperature;
	float resultTimeSpan;

	int colorIndex = 0;
public:
	ProcessProperties(float, float, int, int, float);
	void update();

	void setTimeStep(float timeStep) {
		if (timeStep > targetTimeSpan) {
			this->timeStep = targetTimeSpan;
		}
		else {
			this->timeStep = timeStep;
		}
	}

	void setSnapshotDeltaTime(unsigned int snapshotDeltaTime) { this->snapshotDeltaTime = snapshotDeltaTime; }

	float* getTargetTemperaturePointer() {
		return &targetTemperature;
	}

	int* getTargetTimeSpanPointer() {
		return &targetTimeSpan;
	}

	float* getInitialTemperaturePointer() {
		return &initialTemperature;
	}

	float* getEnvironmentTemperaturePointer() {
		return &environmentTemperature;
	}

	float* getTimeStepPointer() {
		return &timeStep;
	}

	int* getSnapshotIntervalPointer() {
		return &snapshotDeltaTime;
	}

	float getResultTemperature() {
		return resultTemperature;
	}

	float getResultTimeSpan() {
		return resultTimeSpan;
	}

	void setResultTemperature(float resultTemperature) {
		this->resultTemperature = resultTemperature;
	}

	void setResultTimeSpan(float resultTimeSpan) {
		this->resultTimeSpan = resultTimeSpan;
	}

	int* getColorIndexPointer() {
		return &colorIndex;
	}
};
#endif