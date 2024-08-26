#ifndef DRAWER_H
#define DRAWER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <array>

#include <tinycolormap.hpp>
#include <tuple>
#include <string>

class Shader;
class DrinkCooling;
class Snapshot;
class Drawing;

class Drawer {
public:
	inline static const std::array<std::tuple<tinycolormap::ColormapType, std::string>, 14> colors = {
		std::make_tuple(tinycolormap::ColormapType::Heat, "Heat"),
		{tinycolormap::ColormapType::Parula, "Parula"},
		{tinycolormap::ColormapType::Jet, "Jet"},
		{tinycolormap::ColormapType::Turbo, "Turbo"},
		{tinycolormap::ColormapType::Hot, "Hot"},
		{tinycolormap::ColormapType::Gray, "Gray"},
		{tinycolormap::ColormapType::Magma, "Magma"},
		{tinycolormap::ColormapType::Inferno, "Inferno"},
		{tinycolormap::ColormapType::Plasma, "Plasma"},
		{tinycolormap::ColormapType::Viridis, "Viridis"},
		{tinycolormap::ColormapType::Cividis, "Cividis"},
		{tinycolormap::ColormapType::Github, "Github"},
		{tinycolormap::ColormapType::Cubehelix, "Cubehelix"},
		{tinycolormap::ColormapType::HSV, "HSV"}
	};

private:
	Shader* shader;
	glm::mat4 projectionMatrix;

	unsigned int heatMapTextureColorBuffer[colors.size()];
public:
	~Drawer();

	void initialize(int);

	unsigned int getHeatMapTextureColorBuffer(int id) {
		return heatMapTextureColorBuffer[id];
	}

	void updateProjectionMatrix(int, int);

	void visualizeCooling(std::shared_ptr<DrinkCooling> cooling);
	void drawSection(std::shared_ptr<Drawing>, std::shared_ptr<Snapshot>);
	void drawTopOrBottomFace(std::shared_ptr<Drawing>, std::shared_ptr<Snapshot>);
	void drawSide(std::shared_ptr<Drawing>, std::shared_ptr<Snapshot>);
};

#endif