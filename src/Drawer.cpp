#include "Drawer.h"
#include <Shader.h>

#include <tinycolormap.hpp>

#include "DrinkCooling.h"

void Drawer::initialize(int imguiWidth) {
	shader = new Shader("./resources/Drink.vs", "./resources/Drink.fs");

	Shader mapShader = Shader("./resources/Map.vs", "./resources/Map.fs");
	mapShader.use();
	
	//Create framebuffer for colored heatmap
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	int colormapWidth = 3 * imguiWidth / 4;
	glViewport(0, 0, colormapWidth, colormapWidth / 10);

	//Create texturebuffer and the texture where the heatmap will be saved
	glGenTextures(1, &heatMapTextureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, heatMapTextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colormapWidth, colormapWidth / 10, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heatMapTextureColorBuffer, 0);

	//create the vertices for the heatmap (position + color)
	const int numberOfSquares = 20;
	int vertexArraySize = (numberOfSquares + 1) * 2 * (2 + 3);
	float* vertexArray = new float[vertexArraySize];

	int k = 0;
	for (int i = 0; i < numberOfSquares + 1; i++) {
		float factor = float(i) / float(numberOfSquares);
		const tinycolormap::Color color = tinycolormap::GetColor(factor, tinycolormap::ColormapType::Viridis);

		for (int j = 0; j < 2; j++) {
			vertexArray[5 * k + 0] = 2.0f * i / numberOfSquares - 1.0f;
			vertexArray[5 * k + 1] = 2.0f * j - 1.0f;

			vertexArray[5 * k + 2] = float(color.r());
			vertexArray[5 * k + 3] = float(color.g());
			vertexArray[5 * k + 4] = float(color.b());

			k++;
		}
	}

	//create vao, vbo and ebo for the heatmap
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

	//draw the heatmap to the framebuffer and to the attached texture
	glDrawElements(GL_TRIANGLES, numberOfSquares * 6, GL_UNSIGNED_INT, 0);

	//delete vao, ebo, vao
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &framebuffer);

	delete[] vertexArray;
	vertexArray = nullptr;
	delete[] eboArray;
	eboArray = nullptr;	
}

void Drawer::updateProjectionMatrix(int x, int y) {
	projectionMatrix = glm::perspective<float>(glm::radians(90.0f), float(x) / float(y), 0.1f, 200.0f);	
}

void Drawer::visualizeCooling(std::shared_ptr<DrinkCooling> cooling)
{
	shader->use();
	shader->setMat4("cameraMatrix", cooling->getDrawing()->cameraMatrix);
	shader->setMat4("projectionMatrix", projectionMatrix);

	auto drawing = cooling->getDrawing();
	std::shared_ptr<Snapshot> snapshot = cooling->getCurrentSnapshot().lock();

#ifndef __EMSCRIPTEN__
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(5);
#endif
	glLineWidth(2);

	glBindVertexArray(drawing->vao);

	//draw top and bottom faces
	shader->setMat4("modelMatrix", drawing->topFaceModelViewMatrix);
	drawTopOrBottomFace(drawing, snapshot);
	shader->setMat4("modelMatrix", drawing->bottomFaceModelViewMatrix);
	drawTopOrBottomFace(drawing, snapshot);

	//draw two sections
	shader->setMat4("modelMatrix", drawing->defaultModelViewMatrix);
	drawSection(drawing, snapshot);
	shader->setMat4("modelMatrix", drawing->otherSectionModelViewMatrix);
	drawSection(drawing, snapshot);

	//draw side
	shader->setMat4("modelMatrix", drawing->defaultModelViewMatrix);
	drawSide(drawing, snapshot);

	glBindVertexArray(0);
}

void Drawer::drawSection(std::shared_ptr<Drawing> drawing, std::shared_ptr<Snapshot> snapshot) {
	glBindBuffer(GL_ARRAY_BUFFER, snapshot->getVBO());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (drawing->vertexSwitch) {
		shader->setInt("colorMode", 2);
		glDrawArrays(GL_POINTS, 0, drawing->vertexNr);
	}

	if (drawing->lineSwitch) {
		shader->setInt("colorMode", 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawing->lineEbo);
		glDrawElements(GL_LINES, drawing->lineNr, GL_UNSIGNED_INT, 0);
	}

	if (drawing->surfaceSwitch) {
		shader->setInt("colorMode", 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawing->triangleEbo);
		glDrawElements(GL_TRIANGLES, drawing->triangleNr, GL_UNSIGNED_INT, 0);
	}
}

void Drawer::drawTopOrBottomFace(std::shared_ptr<Drawing> drawing, std::shared_ptr<Snapshot> snapshot) {
	//draw top and bottom
	glBindBuffer(GL_ARRAY_BUFFER, snapshot->getTopVbo());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (drawing->vertexSwitch) {
		shader->setInt("colorMode", 2);
		glDrawArrays(GL_POINTS, 0, drawing->topVertexNr);
	}

	if (drawing->lineSwitch) {
		shader->setInt("colorMode", 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawing->topFaceLineEbo);
		glDrawElements(GL_LINES, drawing->topLineNr, GL_UNSIGNED_INT, 0);
	}

	if (drawing->surfaceSwitch) {
		shader->setInt("colorMode", 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawing->topFaceTriangleEbo);
		glDrawElements(GL_TRIANGLES, drawing->topTriangleNr, GL_UNSIGNED_INT, 0);
	}
}

void Drawer::drawSide(std::shared_ptr<Drawing> drawing, std::shared_ptr<Snapshot> snapshot) {
	glBindBuffer(GL_ARRAY_BUFFER, snapshot->getSideVbo());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (drawing->vertexSwitch) {
		shader->setInt("colorMode", 2);
		glDrawArrays(GL_POINTS, 0, drawing->sideVertexNr);
	}

	if (drawing->lineSwitch) {
		shader->setInt("colorMode", 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawing->sideLineEbo);
		glDrawElements(GL_LINES, drawing->sideLineNr, GL_UNSIGNED_INT, 0);
	}

	if (drawing->surfaceSwitch) {
		shader->setInt("colorMode", 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawing->sideTriangleEbo);
		glDrawElements(GL_TRIANGLES, drawing->sideTriangleNr, GL_UNSIGNED_INT, 0);
	}
}

Drawer::~Drawer()
{
	glDeleteTextures(1, &heatMapTextureColorBuffer);
}