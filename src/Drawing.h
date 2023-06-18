#ifndef DRAWING_H
#define DRAWING_H

#include <glm/glm.hpp>
#include "GeometricProperties.h"

#include "Snapshot.h"

#define DEFAULT_CAMERA_DISTANCE 85

class Drawing {
private:
	GeometricProperties* geometricProperties;

	unsigned int* triangleIndices = nullptr;
	unsigned int* lineIndices = nullptr;

	unsigned int* topFaceTriangleIndices = nullptr;
	unsigned int* topFaceLineIndices = nullptr;

	unsigned int* sideTriangleIndices = nullptr;
	unsigned int* sideLineIndices = nullptr;;
	
	//Drawing properties
	bool vertexSwitch = false;
	bool lineSwitch = false;
	bool surfaceSwitch = true;

	unsigned int vao;
	
	unsigned int triangleEbo;
	unsigned int lineEbo;

	unsigned int topFaceTriangleEbo;
	unsigned int topFaceLineEbo;

	unsigned int sideTriangleEbo;
	unsigned int sideLineEbo;

	unsigned int vertexNr;
	unsigned int lineNr;
	unsigned int triangleNr;

	unsigned int topVertexNr;
	unsigned int topLineNr;
	unsigned int topTriangleNr;

	unsigned int sideVertexNr;
	unsigned int sideLineNr;
	unsigned int sideTriangleNr;

	//camera properties
	float cameraDistance = DEFAULT_CAMERA_DISTANCE;
	float cameraAngle = 0;
	float cameraHeight = 0;

	glm::mat4 cameraMatrix;

	glm::mat4 defaultModelViewMatrix = glm::mat4(1.0f);
	glm::mat4 otherSectionModelViewMatrix = glm::mat4(1.0f);

	glm::mat4 topFaceModelViewMatrix;
	glm::mat4 bottomFaceModelViewMatrix;

public:
	~Drawing();
	void setGeometricProperties(GeometricProperties* geometricProperties);
	void setCameraDistance(float cameraDistance);
	void setCameraAngle(float cameraAngle);
	void setCameraHeight(float cameraHeight);
	void determineCameraMatrix();

	void update();
	void destroy();

	float* getCameraDistancePointer() {
		return &cameraDistance;
	}

	float* getCameraAnglePointer() {
		return &cameraAngle;
	}

	float* getCameraHeightPointer() {
		return &cameraHeight;
	}

	bool* getVertexSwitchPointer() {
		return &vertexSwitch;
	}

	bool* getLineSwitchPointer() {
		return &lineSwitch;
	}

	bool* getSurfaceSwitchPointer() {
		return &surfaceSwitch;
	}

	friend class Drawer;
};

#endif // ! DRAWING_H