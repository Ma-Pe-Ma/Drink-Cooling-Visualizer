#ifndef DRAWER_H
#define DRAWER_H

class Shader;
class DrinkCooling;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class Snapshot;
class Drawing;

class Drawer {
	Shader* shader;
	glm::mat4 projectionMatrix;
	unsigned int heatMapTextureColorBuffer;
public:
	~Drawer();

	void initialize(int);

	unsigned int getHeatMapTextureColorBuffer() {
		return heatMapTextureColorBuffer;
	}

	void updateProjectionMatrix(int, int);

	void visualizeCooling(std::shared_ptr<DrinkCooling> cooling);
	void drawSection(std::shared_ptr<Drawing>, std::shared_ptr<Snapshot>);
	void drawTopOrBottomFace(std::shared_ptr<Drawing>, std::shared_ptr<Snapshot>);
	void drawSide(std::shared_ptr<Drawing>, std::shared_ptr<Snapshot>);
};

#endif