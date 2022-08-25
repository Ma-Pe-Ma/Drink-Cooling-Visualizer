#include "Drawing.h"
#include "glm/gtc/matrix_transform.hpp"

Shader* Drawing::shader;
Shader* Drawing::mapShader;
Shader* Drawing::textShader;
glm::mat4 Drawing::projectionMatrix;

unsigned int Drawing::textureColorbuffer;

void Drawing::initializeClass() {
	Drawing::shader = new Shader("resources/Drink.vs", "resources/Drink.fs");
	Drawing::mapShader = new Shader("resources/Map.vs", "resources/Map.fs");

	shader->use();
	projectionMatrix = glm::perspective<float>(glm::radians(90.0f), 1280.0f / 720.0f, 0.1f,200.0f);
	shader->setMat4("projectionMatrix", projectionMatrix);
}

void Drawing::updateProjectionMatrix(int x, int y) {
	projectionMatrix = glm::perspective<float>(glm::radians(90.0f), float(x) / float(y), 0.1f, 200.0f);

	shader->use();
	shader->setMat4("projectionMatrix", projectionMatrix);
}

Drawing::~Drawing() {
	destroy();
}

void Drawing::initializeMap() {
	mapShader->use();
	//Create framebuffer
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glViewport(0, 0, 300, 30);

	//Create texturebuffer
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 300, 30, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//create the points
	int numberOfSquares = 20;
	int vertexArraySize = (numberOfSquares + 1) * 2 * (2 + 3);
	float* vertexArray = new float[vertexArraySize];

	int k = 0;
	for (int i = 0; i < numberOfSquares + 1; i++) {
		float factor = float(i) / float(numberOfSquares);
		const tinycolormap::Color color = tinycolormap::GetColor(factor, tinycolormap::ColormapType::Viridis);

		for (int j = 0; j < 2; j++) {
			vertexArray[5 * k + 0] = 2.0f * i / numberOfSquares - 1.0f;
			vertexArray[k * 5 + 1] = 2.0f * j - 1.0f;

			vertexArray[5 * k + 2] = float(color.r());
			vertexArray[5 * k + 3] = float(color.g());
			vertexArray[5 * k + 4] = float(color.b());

			k++;
		}
	}

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexArraySize, vertexArray, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	int eboSize = numberOfSquares * 6;
	unsigned int* eboArray = new unsigned int[eboSize];

	k = 0;
	for (int i = 0; i < numberOfSquares; i++) {
		eboArray[6 * k + 0] = 2 * i;
		eboArray[6 * k + 1] = 2 * i + 2;
		eboArray[6 * k + 2] = 2 * i + 1;

		eboArray[6 * k + 3] = 2 * i + 2;
		eboArray[6 * k + 4] = 2 * i + 1;
		eboArray[6 * k + 5] = 2 * i + 3;

		k++;
	}

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * eboSize, eboArray, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, numberOfSquares * 6, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	delete[] vertexArray;
	delete[] eboArray;

	glDeleteFramebuffers(1, &framebuffer);
}

void Drawing::destroy() {
	if (triangleIndices != nullptr) {
		delete[] triangleIndices;
	}

	if (lineIndices != nullptr) {
		delete[] lineIndices;
	}

	if (topFaceTriangleIndices != nullptr) {
		delete[] topFaceTriangleIndices;
	}

	if (topFaceLineIndices != nullptr) {
		delete[] topFaceLineIndices;
	}

	if (sideTriangleIndices != nullptr) {
		delete[] sideTriangleIndices;
	}

	if (sideLineIndices != nullptr) {
		delete[] sideLineIndices;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &triangleEbo);
	glDeleteBuffers(1, &lineEbo);

	glDeleteBuffers(1, &topFaceTriangleEbo);
	glDeleteBuffers(1, &topFaceLineEbo);	

	glDeleteBuffers(1, &sideTriangleEbo);
	glDeleteBuffers(1, &sideLineEbo);
}

Drawing::Drawing() {
	
}

void Drawing::setGeometricProperties(GeometricProperties* geometricProperties) {
	this->geometricProperties = geometricProperties;
}

void Drawing::update() {
	destroy();

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

	this->verticeNr = radiusPointNr * axisPointNr;
	this->lineNr = 2 * (axisPointNr + radiusPointNr);
	this->triangleNr = (radiusPointNr - 1) * (axisPointNr - 1) * 6;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &triangleEbo);
	glGenBuffers(1, &lineEbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangleIndiceNr, triangleIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * lineIndiceNr, lineIndices, GL_STATIC_DRAW);

	int sectionAngle = geometricProperties->getSectionAngle();

	otherSectionModelViewMatrix = glm::mat4(1.0f);
	otherSectionModelViewMatrix = glm::rotate(otherSectionModelViewMatrix, glm::radians(float(sectionAngle)), glm::vec3(0, 1, 0));

	std::cout << "Drawing updated!";

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

	this->topVerticeNr = (radiusPointNr - 1) * topPointNr + 1;
	this->topLineNr = (radiusPointNr - 1) * topSectionNr * 2;
	this->topTriangleNr = 3 * topSectionNr + (radiusSectionNr - 1) * topSectionNr * 6;

	glGenBuffers(1, &topFaceTriangleEbo);
	glGenBuffers(1, &topFaceLineEbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFaceTriangleEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * topFaceTriangleIndiceNr, topFaceTriangleIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFaceLineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * topFaceLineIndiceNr, topFaceLineIndices, GL_STATIC_DRAW);
	
	float axisLength = geometricProperties->getAxisLength();

	topFaceModelViewMatrix = glm::mat4(1.0f);
	topFaceModelViewMatrix = glm::translate(topFaceModelViewMatrix, glm::vec3(0, axisLength / 2, 0));
	topFaceModelViewMatrix = glm::rotate(topFaceModelViewMatrix, glm::radians(sectionAngle / 2.0f - 90), glm::vec3(0, 1, 0));
	topFaceModelViewMatrix = glm::rotate(topFaceModelViewMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));	

	bottomFaceModelViewMatrix = glm::mat4(1.0f);
	bottomFaceModelViewMatrix = glm::translate(bottomFaceModelViewMatrix, glm::vec3(0, -axisLength / 2, 0));
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

	this->sideVerticeNr = axisPointNr * topPointNr;
	this->sideLineNr = axisPointNr * topSectionNr * 2;
	this->sideTriangleNr = axisSectionNr * topSectionNr * 6;

	glGenBuffers(1, &sideTriangleEbo);
	glGenBuffers(1, &sideLineEbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideTriangleEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sideTriangleIndiceNr, sideTriangleIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideLineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sideLineIndiceNr, sideLineIndices, GL_STATIC_DRAW);
}

void Drawing::visualize(std::shared_ptr<Snapshot> snapshot) {
	shader->use();
	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glLineWidth(2);
	glPointSize(5);

	glBindVertexArray(vao);

	//draw top and bottom faces
	shader->setMat4("modelMatrix", topFaceModelViewMatrix);
	drawTopOrBottomFace(snapshot);
	shader->setMat4("modelMatrix", bottomFaceModelViewMatrix);
	drawTopOrBottomFace(snapshot);

	//draw two sections
	shader->setMat4("modelMatrix", defaultModelViewMatrix);
	drawSection(snapshot);
	shader->setMat4("modelMatrix", otherSectionModelViewMatrix);
	drawSection(snapshot);

	//draw side
	shader->setMat4("modelMatrix", defaultModelViewMatrix);
	drawSide(snapshot);
	
	glBindVertexArray(0);
}

void Drawing::drawSection(std::shared_ptr<Snapshot> snapshot) {
	glBindBuffer(GL_ARRAY_BUFFER, snapshot->getVBO());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (verticeSwitch) {
		shader->setInt("colorMode", 2);
		glDrawArrays(GL_POINTS, 0, this->verticeNr);
	}

	if (lineSwitch) {
		shader->setInt("colorMode", 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEbo);
		glDrawElements(GL_LINES, this->lineNr, GL_UNSIGNED_INT, 0);
	}

	if (surfaceSwitch) {
		shader->setInt("colorMode", 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEbo);
		glDrawElements(GL_TRIANGLES, this->triangleNr, GL_UNSIGNED_INT, 0);
	}
}

void Drawing::drawTopOrBottomFace(std::shared_ptr<Snapshot> snapshot) {
	//draw top and bottom
	glBindBuffer(GL_ARRAY_BUFFER, snapshot->getTopVbo());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (verticeSwitch) {
		shader->setInt("colorMode", 2);
		glDrawArrays(GL_POINTS, 0, this->topVerticeNr);
	}

	if (lineSwitch) {
		shader->setInt("colorMode", 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFaceLineEbo);
		glDrawElements(GL_LINES, this->topLineNr, GL_UNSIGNED_INT, 0);
	}

	if (surfaceSwitch) {
		shader->setInt("colorMode", 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFaceTriangleEbo);
		glDrawElements(GL_TRIANGLES, this->topTriangleNr, GL_UNSIGNED_INT, 0);
	}
}

void Drawing::drawSide(std::shared_ptr<Snapshot> snapshot) {
	glBindBuffer(GL_ARRAY_BUFFER, snapshot->getSideVbo());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (verticeSwitch) {
		shader->setInt("colorMode", 2);
		glDrawArrays(GL_POINTS, 0, this->sideVerticeNr);
	}

	if (lineSwitch) {
		shader->setInt("colorMode", 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideLineEbo);
		glDrawElements(GL_LINES, this->sideLineNr, GL_UNSIGNED_INT, 0);
	}

	if (surfaceSwitch) {
		shader->setInt("colorMode", 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sideTriangleEbo);
		glDrawElements(GL_TRIANGLES, this->sideTriangleNr, GL_UNSIGNED_INT, 0);
	}	
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

	shader->use();
	shader->setMat4("cameraMatrix", cameraMatrix);
}