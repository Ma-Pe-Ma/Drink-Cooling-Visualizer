#include "GeometricProperties.h"
#include <math.h>

#define MAGIC_NUMBER sqrt(PI) 
//#define USE_EQUAL_DISCRETE_VOLUMES

GeometricProperties::GeometricProperties(GeometricParameters geometricParameters) {
	this->geometricParameters = geometricParameters;
	this->guiGeometricParameters = geometricParameters;
}

void GeometricProperties::update() {
	this->destroy();

	this->geometricParameters = this->guiGeometricParameters;

	this->radiusPointNr = this->geometricParameters.radiusSectionNr + 1;
	this->axisPointNr = this->geometricParameters.axisSectionNr + 1;

	radiusPoints = new float[radiusPointNr];
	for (int i = 0; i < radiusPointNr; i++) {
#ifdef USE_EQUAL_DISCRETE_VOLUMES
		radiusPoints[i] = this->geometricParameters.radiusLength * sqrt(float(i) / this->geometricParameters.radiusSectionNr);
#else
		radiusPoints[i] = float(i) * this->geometricParameters.radiusLength / this->geometricParameters.radiusSectionNr;
#endif // USE_EQUAL_DISCRETE_VOLUMES
	}

	radiusSectionLengths = new float[this->geometricParameters.radiusSectionNr + 1];
	for (int i = 0; i < this->geometricParameters.radiusSectionNr; i++) {
		radiusSectionLengths[i] = radiusPoints[i + 1] - radiusPoints[i];
	}

	radiusFactor = new float[this->geometricParameters.radiusSectionNr];
	for (int i = 0; i < this->geometricParameters.radiusSectionNr; i++) {
		radiusFactor[i] = (2.0f / (radiusPoints[i] + radiusPoints[i + 1]) + 1.0f / radiusSectionLengths[i]) / radiusSectionLengths[i];
	}

	axisPoints = new float[axisPointNr];
	float axisDistance = this->geometricParameters.axisLength / this->geometricParameters.axisSectionNr;
	for (int i = 0; i < axisPointNr; i++) {
		axisPoints[i] = -this->geometricParameters.axisLength / 2 + i * axisDistance;
	}

	axisSectionLengths = new float[this->geometricParameters.axisSectionNr];
	for (int i = 0; i < this->geometricParameters.axisSectionNr; i++) {
		axisSectionLengths[i] = axisPoints[i + 1] - axisPoints[i];
	}

	this->volume = (PI * this->geometricParameters.radiusLength * this->geometricParameters.radiusLength * this->geometricParameters.axisLength);

	//TODO: special section for heat transfer at edge for cylinder surface
#ifdef MAGIC_NUMBER
	radiusSectionLengths[this->geometricParameters.radiusSectionNr] = axisSectionLengths[this->geometricParameters.axisSectionNr - 1] * MAGIC_NUMBER;
#else
	radiusSectionLengths[this->geometricParameters.radiusSectionNr] = [this]() -> float {
		float r = radiusPoints[radiusPointNr - 1];
		float dr = r - radiusPoints[radiusPointNr - 2];

		return  dr * (1 - dr / (2 * r));
		}();
#endif // MAGIC_NUMBER	
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

	if (radiusFactor != nullptr) {
		delete[] radiusFactor;
		radiusFactor = nullptr;
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