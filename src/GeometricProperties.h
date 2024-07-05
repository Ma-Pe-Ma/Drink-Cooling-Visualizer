#ifndef GEOMETRICPROPERTIES_H
#define GEOMETRICPROPERTIES_H

#define PI 3.14159265359f
#define TOP_SECTION_DISCRETISATION_FACTOR 2

struct GeometricParameters {
	int radiusSectionNr;
	int axisSectionNr;
	float radiusLength;
	float axisLength;
	int sectionAngle = 60;
};

class GeometricProperties {
	GeometricParameters geometricParameters;
	GeometricParameters guiGeometricParameters;

	int radiusPointNr;	
	int axisPointNr;

	float* radiusPoints = nullptr;
	float* radiusSectionLengths = nullptr;
	float* radiusFactor = nullptr;

	float* axisPoints = nullptr;
	float* axisSectionLengths = nullptr;

	float volume;
public:
	GeometricProperties(GeometricParameters);
	~GeometricProperties();

	void update();
	void destroy();
		
	int getRadiusPointNr() {
		return radiusPointNr;
	}

	int getAxisPointNr() {
		return axisPointNr;
	}

	float* getRadiusPointsPointer() {
		return radiusPoints;
	}

	float* getAxisPointsPointer() {
		return axisPoints;
	}

	float* getRadiusSectionsPointer() {
		return radiusSectionLengths;
	}

	float* getAxisSectionsPointer() {
		return axisSectionLengths;
	}

	float* getRadiusFactor() {
		return radiusFactor;
	}

	float getVolume() {
		return volume;
	}

	GeometricParameters& getGeometricParameters() {
		return this->geometricParameters;
	}

	GeometricParameters& getGUIGeometricParameters() {
		return this->guiGeometricParameters;
	}
};

#endif