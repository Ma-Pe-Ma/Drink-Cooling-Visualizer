#include "GeometricProperties.h"
#include <math.h>

GeometricProperties::GeometricProperties(float radiusLength, float axisLength, int radiusSectionNr, int axisSectionNr, int sectionAngle) {
	this->radiusLength = radiusLength;
	this->axisLength = axisLength;
	this->radiusSectionNr = radiusSectionNr;
	this->axisSectionNr = axisSectionNr;
	this->sectionAngle = sectionAngle;

	this->radiusLengthGUI = radiusLength;
	this->axisLengthGUI = axisLength;
	this->radiusSectionNrGUI = radiusSectionNr;
	this->axisSectionNrGUI = axisSectionNr;
	this->sectionAngleGUI = sectionAngle;
}

void GeometricProperties::update() {
	this->destroy();

	this->radiusLength = radiusLengthGUI;
	this->axisLength = axisLengthGUI;

	this->radiusSectionNr = radiusSectionNrGUI;
	this->axisSectionNr = axisSectionNrGUI;

	this->sectionAngle = sectionAngleGUI;

	this->radiusPointNr = radiusSectionNr + 1;
	this->axisPointNr = axisSectionNr + 1;	

	radiusPoints = new float[radiusPointNr];
	for (int i = 0; i < radiusPointNr; i++) {
		radiusPoints[i] = radiusLength * sqrt(float(i) / radiusSectionNr);
	}

	radiusSectionLengths = new float[radiusSectionNr + 1];
	for (int i = 0; i < radiusSectionNr; i++) {
		radiusSectionLengths[i] = radiusPoints[i + 1] - radiusPoints[i];
	}

	//special section for heat transfer at edge
	//radiusSectionLengths[radiusSectionNr] = 0.5f * (sqrt(1 + 1.0f / radiusSectionNr) - sqrt(1 - 1.0f / radiusSectionNr));
	radiusSectionLengths[radiusSectionNr] = 1.0f;

	axisPoints = new float[axisPointNr];
	float axisDistance = axisLength / axisSectionNr;
	for (int i = 0; i < axisPointNr; i++) {
		axisPoints[i] = -axisLength / 2 + i * axisDistance;
	}

	axisSectionLengths = new float[axisSectionNr];
	for (int i = 0; i < axisSectionNr; i++) {
		axisSectionLengths[i] = axisPoints[i + 1] - axisPoints[i];
	}

	this->volume = (PI * radiusLength * radiusLength * axisLength);
}

void GeometricProperties::destroy() {
	if (radiusPoints != nullptr) {
		delete[] radiusPoints;
		radiusPoints = nullptr;
	}

	if (radiusSectionLengths != nullptr) {
		delete[] radiusSectionLengths;
		radiusSectionLengths = nullptr;
	}

	if (axisPoints != nullptr) {
		delete[] axisPoints;
		axisPoints = nullptr;
	}

	if (axisSectionLengths != nullptr) {
		delete[] axisSectionLengths;
		axisSectionLengths = nullptr;
	}	
}

GeometricProperties::~GeometricProperties() {
	this->destroy();
}