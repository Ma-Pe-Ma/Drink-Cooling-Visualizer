#ifndef DRAWING_H
#define DRAWING_H

#include <Shader.h>
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

	static Shader* shader;
	static Shader* mapShader;
	static Shader* textShader;

	glm::mat4 cameraMatrix;
	static glm::mat4 projectionMatrix;
	
	glm::mat4 defaultModelViewMatrix = glm::mat4(1.0f);
	glm::mat4 otherSectionModelViewMatrix = glm::mat4(1.0f);

	glm::mat4 topFaceModelViewMatrix;
	glm::mat4 bottomFaceModelViewMatrix;

	//camera properties
	float cameraDistance = DEFAULT_CAMERA_DISTANCE;
	float cameraAngle = 0;
	float cameraHeight = 0;

	//Drawing properties
	bool verticeSwitch = false;
	bool lineSwitch = false;
	bool surfaceSwitch = true;

	unsigned int vao;
	
	unsigned int triangleEbo;
	unsigned int lineEbo;

	unsigned int topFaceTriangleEbo;
	unsigned int topFaceLineEbo;

	unsigned int sideTriangleEbo;
	unsigned int sideLineEbo;

	unsigned int verticeNr;
	unsigned int lineNr;
	unsigned int triangleNr;

	unsigned int topVerticeNr;
	unsigned int topLineNr;
	unsigned int topTriangleNr;

	unsigned int sideVerticeNr;
	unsigned int sideLineNr;
	unsigned int sideTriangleNr;

	static unsigned int textureColorbuffer;

public:
	static void initializeClass();
	static void updateProjectionMatrix(int, int);

	Drawing();
	~Drawing();
	void setGeometricProperties(GeometricProperties* geometricProperties);
	void setCameraDistance(float cameraDistance);
	void setCameraAngle(float cameraAngle);
	void setCameraHeight(float cameraHeight);
	void determineCameraMatrix();


	float* getCameraDistancePointer() {
		return &cameraDistance;
	}

	float* getCameraAnglePointer() {
		return &cameraAngle;
	}

	float* getCameraHeightPointer() {
		return &cameraHeight;
	}

	bool* getVerticesSwitchPointer() {
		return &verticeSwitch;
	}

	bool* getLineSwitchPointer() {
		return &lineSwitch;
	}

	bool* getSurfaceSwitch() {
		return &surfaceSwitch;
	}

	void update();
	void destroy();

	void visualize(std::shared_ptr<Snapshot>);

	void drawTopOrBottomFace(std::shared_ptr<Snapshot>);
	void drawSide(std::shared_ptr<Snapshot>);
	void drawSection(std::shared_ptr<Snapshot>);

	static void initializeMap();

	static unsigned int getTexure() {
		return textureColorbuffer;
	}
};

#endif // ! DRAWING_H