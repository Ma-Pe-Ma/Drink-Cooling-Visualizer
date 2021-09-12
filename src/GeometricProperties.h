#ifndef GEOMETRICPROPERTIES_H
#define GEOMETRICPROPERTIES_H

#define PI 3.14159265359f

class GeometricProperties {
	//geometry properties
	int radiusSectionNr;
	int radiusPointNr;
	int axisSectionNr;
	int axisPointNr;

	float sectionAngle = 60;

	float radiusLength;
	float axisLength;

	float* radiusPoints;
	float* radiusSectionLengths;

	float* axisPoints;
	float* axisSectionLengths;

	float volume;

	float factor = 2;

	int radiusSectionNrGUI;
	int axisSectionNrGUI;
	float radiusLengthGUI;
	float axisLengthGUI;
	int sectionAngleGUI;

public:
	GeometricProperties(float, float, int, int, int);
	~GeometricProperties();
		
	int getRadiusPointNr() {
		return radiusPointNr;
	}

	int getAxisPointNr() {
		return axisPointNr;
	}

	int getRadiusSectionNr() {
		return radiusSectionNr;
	}

	int getAxisSectiontNr() {
		return axisSectionNr;
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

	float getSectionAnglePointer() {
		return sectionAngle;
	}

	float getVolume() {
		return volume;
	}

	float getRadiusLength() {
		return radiusLength;
	}

	float getAxisLength() {
		return axisLength;
	}

	float getSectionAngle() {
		return sectionAngle;
	}

	float getFactor() {
		return factor;
	}

	//gui value holders
	int* getRadiusNrGUI() {
		return &radiusSectionNrGUI;
	}

	int* getAxisNrGUI() {
		return &axisSectionNrGUI;
	}

	float* getRadiusLengthGUI() {
		return &radiusLengthGUI;
	}

	float* getAxisLengthGUI() {
		return &axisLengthGUI;
	}

	int* getSectionAngleGUI() {
		return &sectionAngleGUI;
	}

	void destroy();
	void update();		
};

#endif