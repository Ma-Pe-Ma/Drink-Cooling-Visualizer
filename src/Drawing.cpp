#include "Drawing.h"
#include "glm/gtc/matrix_transform.hpp"

Drawing::~Drawing() {
	destroy();
}

void Drawing::setGeometricProperties(GeometricProperties* geometricProperties) {
	this->geometricProperties = geometricProperties;
}

void Drawing::update() {
	this->destroy();
	glGenVertexArrays(1, &vao);

	int radiusSectionNr = geometricProperties->getRadiusSectionNr();
	int axisSectionNr = geometricProperties->getAxisSectiontNr();
	int radiusPointNr = geometricProperties->getRadiusPointNr();
	int axisPointNr= geometricProperties->getAxisPointNr();

	int triangleIndiceNr = 6 * radiusSectionNr * axisSectionNr;
	triangleIndices = new unsigned int[triangleIndiceNr];

	int k = 0;
	for (int j = 0; j < axisSectionNr; j++) {
		for (int i = 0; i < radiusSectionNr; i++) {
			triangleIndices[6 * k + 0] = (j + 0) * radiusPointNr + i + 0;
			triangleIndices[6 * k + 1] = (j + 0) * radiusPointNr + i + 1;
			triangleIndices[6 * k + 2] = (j + 1) * radiusPointNr + i + 0;

			triangleIndices[6 * k + 3] = (j + 0) * radiusPointNr + i + 1;
			triangleIndices[6 * k + 4] = (j + 1) * radiusPointNr + i + 0;
			triangleIndices[6 * k + 5] = (j + 1) * radiusPointNr + i + 1;

			k++;
		}
	}

	glGenBuffers(1, &triangleEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangleIndiceNr, triangleIndices, GL_STATIC_DRAW);
	delete[] triangleIndices;
	triangleIndices = nullptr;

	int lineIndiceNr = (axisPointNr + radiusPointNr) * 2;
	lineIndices = new unsigned int[lineIndiceNr];

	k = 0;
	for (int i = 0; i < radiusPointNr; i++) {
		lineIndices[k * 2 + 0] = i;
		lineIndices[k * 2 + 1] = axisPointNr * radiusPointNr - radiusPointNr + i;
		k++;
	}

	for (int i = 0; i < axisPointNr; i++) {
		lineIndices[k * 2 + 0] = i * radiusPointNr;
		lineIndices[k * 2 + 1] = (i + 1) * radiusPointNr - 1;
		k++;
	}

	this->vertexNr = radiusPointNr * axisPointNr;
	this->lineNr = 2 * (axisPointNr + radiusPointNr);
	this->triangleNr = (radiusPointNr - 1) * (axisPointNr - 1) * 6;

	glGenBuffers(1, &lineEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * lineIndiceNr, lineIndices, GL_STATIC_DRAW);
	delete[] lineIndices;
	lineIndices = nullptr;

	int sectionAngle = geometricProperties->getSectionAngle();

	otherSectionModelViewMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(float(sectionAngle)), glm::vec3(0, 1, 0));

	//std::cout << "Drawing updated!";

	unsigned int factor = geometricProperties->getFactor();
	float radiusLength = geometricProperties->getRadiusLength();

	unsigned int topSectionNr = (int) (radiusLength * PI * sectionAngle / 180 * factor);
	unsigned int topPointNr = topSectionNr + 1;

	int topFaceTriangleIndiceNr = topSectionNr * 3 + 6 * (radiusSectionNr - 1) * topSectionNr;
	topFaceTriangleIndices = new unsigned int[topFaceTriangleIndiceNr];

	k = 0;
	for (int i = 0; i < topSectionNr; i++) {
		topFaceTriangleIndices[3 * k + 0] = 0;
		topFaceTriangleIndices[3 * k + 1] = i + 1;
		topFaceTriangleIndices[3 * k + 2] = i + 2;
		k++;
	}

	int l = 0;
	for (int i = 1; i < radiusSectionNr; i++) {
		for (int j = 0; j < topSectionNr; j++) {
			topFaceTriangleIndices[k * 3 + 6 * l + 0] = (i - 1) * topPointNr + j + 0 + 1;
			topFaceTriangleIndices[k * 3 + 6 * l + 1] = (i - 1) * topPointNr + j + 1 + 1;
			topFaceTriangleIndices[k * 3 + 6 * l + 2] = (i - 0) * topPointNr + j + 0 + 1;

			topFaceTriangleIndices[k * 3 + 6 * l + 3] = (i - 1) * topPointNr + j + 1 + 1;
			topFaceTriangleIndices[k * 3 + 6 * l + 4] = (i - 0) * topPointNr + j + 0 + 1;
			topFaceTriangleIndices[k * 3 + 6 * l + 5] = (i - 0) * topPointNr + j + 1 + 1;
			l++;
		}
	}

	glGenBuffers(1, &topFaceTriangleEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFaceTriangleEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * topFaceTriangleIndiceNr, topFaceTriangleIndices, GL_STATIC_DRAW);
	delete[] topFaceTriangleIndices;
	topFaceTriangleIndices = nullptr;

	int topFaceLineIndiceNr = topSectionNr * (radiusPointNr - 1) * 2;
	topFaceLineIndices = new unsigned int[topFaceLineIndiceNr];

	k = 0;
	for (int i = 1; i < radiusPointNr; i++) {
		for (int j = 0; j < topSectionNr; j++) {
			topFaceLineIndices[2 * k + 0] = 1 + (i - 1) * topPointNr + j;
			topFaceLineIndices[2 * k + 1] = 1 + (i - 1) * topPointNr + j + 1;
			k++;
		}
	}

	this->topVertexNr = (radiusPointNr - 1) * topPointNr + 1;
	this->topLineNr = (radiusPointNr - 1) * topSectionNr * 2;
	this->topTriangleNr = 3 * topSectionNr + (radiusSectionNr - 1) * topSectionNr * 6;

	glGenBuffers(1, &topFaceLineEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFaceLineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * topFaceLineIndiceNr, topFaceLineIndices, GL_STATIC_DRAW);
	delete[] topFaceLineIndices;
	topFaceLineIndices = nullptr;

	float axisLength = geometricProperties->getAxisLength();

	topFaceModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, axisLength / 2, 0));
	topFaceModelViewMatrix = glm::rotate(topFaceModelViewMatrix, glm::radians(sectionAngle / 2.0f - 90), glm::vec3(0, 1, 0));
	topFaceModelViewMatrix = glm::rotate(topFaceModelViewMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));	

	bottomFaceModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -axisLength / 2, 0));
	bottomFaceModelViewMatrix = glm::rotate(bottomFaceModelViewMatrix, glm::radians(sectionAngle / 2.0f - 90), glm::vec3(0, 1, 0));
	bottomFaceModelViewMatrix = glm::rotate(bottomFaceModelViewMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));

	int sideTriangleIndiceNr = 6 * axisSectionNr * topSectionNr;
	sideTriangleIndices = new unsigned int[sideTriangleIndiceNr];

	k = 0;
	for (int i = 0; i < axisSectionNr; i++) {
		for (int j = 0; j < topSectionNr; j++) {
			sideTriangleIndices[6 * k + 0] = (i + 0) * topPointNr + j + 0;
			sideTriangleIndices[6 * k + 1] = (i + 0) * topPointNr + j + 1;
			sideTriangleIndices[6 * k + 2] = (i + 1) * topPointNr + j + 0;

			sideTriangleIndices[6 * k + 3] = (i + 0) * topPointNr + j + 1;
			sideTriangleIndices[6 * k + 4] = (i + 1) * topPointNr + j + 0;
			sideTriangleIndices[6 * k + 5] = (i + 1) * topPointNr + j + 1;
			k++;
		}
	}

	glGenBuffers(1, &sideTriangleEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideTriangleEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sideTriangleIndiceNr, sideTriangleIndices, GL_STATIC_DRAW);
	delete[] sideTriangleIndices;
	sideTriangleIndices = nullptr;

	int sideLineIndiceNr = axisPointNr * topSectionNr * 2;
	sideLineIndices = new unsigned int[sideLineIndiceNr];
	
	k = 0;
	for (int i = 0; i < axisPointNr; i++) {
		for (int j = 0; j < topSectionNr; j++) {
			sideLineIndices[k * 2 + 0] = i * topPointNr + j;
			sideLineIndices[k * 2 + 1] = i * topPointNr + j + 1;
			k++;
		}
	}

	this->sideVertexNr = axisPointNr * topPointNr;
	this->sideLineNr = axisPointNr * topSectionNr * 2;
	this->sideTriangleNr = axisSectionNr * topSectionNr * 6;

	glGenBuffers(1, &sideLineEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideLineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sideLineIndiceNr, sideLineIndices, GL_STATIC_DRAW);
	delete[] sideLineIndices;
	sideLineIndices = nullptr;
}

void Drawing::setCameraDistance(float cameraDistance) {
	this->cameraDistance = cameraDistance;
	determineCameraMatrix();
}

void Drawing::setCameraAngle(float cameraAngle) {
	this->cameraAngle = cameraAngle;
	determineCameraMatrix();
}

void Drawing::setCameraHeight(float cameraHeight) {
	this->cameraHeight = cameraHeight;
	determineCameraMatrix();
}

void Drawing::determineCameraMatrix() {
	glm::vec3 position(cameraDistance * sin(glm::radians(cameraAngle)), cameraHeight, cameraDistance * cos(glm::radians(cameraAngle)));
	glm::vec3 direction = -glm::normalize(glm::vec3(position[0], 0, position[2]));
	cameraMatrix = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}

void Drawing::destroy() {
	if (triangleIndices != nullptr) {
		delete[] triangleIndices;
		triangleIndices = nullptr;
	}	

	if (lineIndices != nullptr) {
		delete[] lineIndices;
		lineIndices = nullptr;
	}

	if (topFaceTriangleIndices != nullptr) {
		delete[] topFaceTriangleIndices;
		topFaceTriangleIndices = nullptr;
	}	

	if (topFaceLineIndices != nullptr) {
		delete[] topFaceLineIndices;
		topFaceLineIndices = nullptr;
	}	

	if (sideTriangleIndices != nullptr) {
		delete[] sideTriangleIndices;
		sideTriangleIndices = nullptr;
	}	

	if (sideLineIndices != nullptr) {
		delete[] sideLineIndices;
		sideLineIndices = nullptr;
	}	

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &triangleEbo);
	glDeleteBuffers(1, &lineEbo);

	glDeleteBuffers(1, &topFaceTriangleEbo);
	glDeleteBuffers(1, &topFaceLineEbo);

	glDeleteBuffers(1, &sideTriangleEbo);
	glDeleteBuffers(1, &sideLineEbo);
}