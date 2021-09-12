#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <memory>
#include <glad/glad.h>
#include <tinycolormap.hpp>
#include <iostream>

#include <glm/glm.hpp>

#include "GeometricProperties.h"

class Snapshot {
	float* vertexArray;
	float* topVertexArray;
	float* sideVertexArray;

	int vertexSize;
	int topVertexSize;
	int sideVertexSize;

	float averageTemperature;
	const int usedVertexNr = 6;

	unsigned int vbo = 0;
	unsigned int topVbo = 0;
	unsigned int sideVbo = 0;

	GeometricProperties* geometricProperties;
	int time;
public:
	Snapshot(float**&, float, float, float, int, GeometricProperties*);
	
	void draw();

	~Snapshot();

	unsigned int getVBO() {
		if (vbo == 0) {
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexSize, this->vertexArray, GL_STATIC_DRAW);

			delete[] vertexArray;
			vertexArray = nullptr;
		}

		return vbo;
	}

	unsigned int getTopVbo() {
		if (topVbo == 0) {
			glGenBuffers(1, &topVbo);
			glBindBuffer(GL_ARRAY_BUFFER, topVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * topVertexSize, this->topVertexArray, GL_STATIC_DRAW);

			delete[] topVertexArray;
			topVertexArray = nullptr;
		}

		return topVbo;
	}

	unsigned int getSideVbo() {
		if (sideVbo == 0) {
			glGenBuffers(1, &sideVbo);
			glBindBuffer(GL_ARRAY_BUFFER, sideVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sideVertexSize, this->sideVertexArray, GL_STATIC_DRAW);
			
			delete[] sideVertexArray;
			sideVertexArray = nullptr;
		}

		return sideVbo;
	}

	float getAverageTemperature() {
		return averageTemperature;
	}

	int getTime() {
		return time;
	}
};

#endif