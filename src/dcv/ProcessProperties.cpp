#include "ProcessProperties.h"

ProcessProperties::ProcessProperties(float initialTemperature, float environmentTemperature, int targetTimeSpan, int snapshotDeltaTime, float timeStep) {
	this->initialTemperature = initialTemperature;
	this->environmentTemperature = environmentTemperature;
	this->targetTemperature = environmentTemperature + 0.5f;
	
	this->targetTimeSpan = targetTimeSpan;
	this->snapshotDeltaTime = snapshotDeltaTime;
	this->timeStep = timeStep;
}

void ProcessProperties::update() {
	initialTemperature = initialTemperature > 100 ? 100 : initialTemperature;
	initialTemperature = initialTemperature < 0 ? 0 : initialTemperature;
	environmentTemperature = environmentTemperature > initialTemperature -1 ? initialTemperature - 1 : environmentTemperature;
	environmentTemperature = environmentTemperature < -50 ? -50 : environmentTemperature;
	targetTemperature = targetTemperature < environmentTemperature + 0.5f ? environmentTemperature + 0.5f : targetTemperature;
	targetTemperature = targetTemperature > initialTemperature -0.5f ? initialTemperature - 0.5f : targetTemperature;

	snapshotDeltaTime = snapshotDeltaTime > 60 ? 60: snapshotDeltaTime;
	snapshotDeltaTime = snapshotDeltaTime < 5 ? 5 : snapshotDeltaTime;

	targetTimeSpan = targetTimeSpan < 1 ? 1 : targetTimeSpan;
	targetTimeSpan = targetTimeSpan > 720 ? 720 : targetTimeSpan;
}