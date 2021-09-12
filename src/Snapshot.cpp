#include "Snapshot.h"

Snapshot::Snapshot(float**& heatMap, float averageTemperature, float initTemp, float environmentTemp, int time ,GeometricProperties* geometricProperties) {
	this->averageTemperature = averageTemperature;
	this->time = time;

	int radiusNr = geometricProperties->getRadiusPointNr();
	int axisNr = geometricProperties->getAxisPointNr();

	float* radiusPoints = geometricProperties->getRadiusPointsPointer();
	float* axisPoints = geometricProperties->getAxisPointsPointer();

	this->vertexSize = axisNr * radiusNr * this->usedVertexNr;
	this->vertexArray = new float[vertexSize];

	int k = 0;

	for (int j = 0; j < axisNr; j++) {
		for (int i = 0; i < radiusNr; i++) {
			float heatFactor = (heatMap[i][j] - environmentTemp) / (initTemp - environmentTemp);
			
			float randColor = 0.0f + float(rand()% 11) * 0.1f;
			const tinycolormap::Color color = tinycolormap::GetColor(heatFactor, tinycolormap::ColormapType::Viridis);
			//const tinycolormap::Color color = tinycolormap::GetColor(randColor, tinycolormap::ColormapType::Viridis);

			vertexArray[k * usedVertexNr + 0] = radiusPoints[i];
			vertexArray[k * usedVertexNr + 1] = axisPoints[j];
			vertexArray[k * usedVertexNr + 2] = 0;

			vertexArray[k * usedVertexNr + 3] = (float)color.r();
			vertexArray[k * usedVertexNr + 4] = (float)color.g();
			vertexArray[k * usedVertexNr + 5] = (float)color.b();

			k++;
		}
	}
	
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexSize, this->vertexArray, GL_STATIC_DRAW);

	float radiusLength = geometricProperties->getRadiusLength();
	float factor = geometricProperties->getFactor();

	float sectionAngleRadian = PI * geometricProperties->getSectionAngle() / 180;
	unsigned int topSectionNr = (int) radiusLength * sectionAngleRadian * factor;
	unsigned int topPointNr = topSectionNr + 1;

	this->topVertexSize = usedVertexNr * (1 + (radiusNr - 1) * topPointNr);
	topVertexArray = new float[topVertexSize];

	topVertexArray[0] = 0;
	topVertexArray[1] = 0;
	topVertexArray[2] = 0;

	float heatFactor = (heatMap[0][geometricProperties->getAxisPointNr() - 1] - environmentTemp) / (initTemp - environmentTemp);
	const tinycolormap::Color color = tinycolormap::GetColor(heatFactor, tinycolormap::ColormapType::Viridis);

	topVertexArray[3] = (float) color.r();
	topVertexArray[4] = (float) color.g();
	topVertexArray[5] = (float) color.b();

	k = 1;
	for (int j = 1; j < radiusNr; j++) {
		float heatFactor = (heatMap[j][geometricProperties->getAxisPointNr() - 1] - environmentTemp) / (initTemp - environmentTemp);
		const tinycolormap::Color color = tinycolormap::GetColor(heatFactor, tinycolormap::ColormapType::Viridis);

		for (int i = 0; i < topPointNr; i++) {
			topVertexArray[k * usedVertexNr + 0] = radiusPoints[j] * sin(-sectionAngleRadian / 2 + i * sectionAngleRadian / topSectionNr);
			topVertexArray[k * usedVertexNr + 1] = radiusPoints[j] * cos(-sectionAngleRadian / 2 + i * sectionAngleRadian / topSectionNr);
			topVertexArray[k * usedVertexNr + 2] = 0;

			topVertexArray[k * usedVertexNr + 3] = (float) color.r();
			topVertexArray[k * usedVertexNr + 4] = (float) color.g();
			topVertexArray[k * usedVertexNr + 5] = (float) color.b();

			k++;
		}		
	}

	//glGenBuffers(1, &topVbo);
	//glBindBuffer(GL_ARRAY_BUFFER, topVbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * topVertexSize, this->topVertexArray, GL_STATIC_DRAW);


	float axisLength = geometricProperties->getAxisLength();
	this->sideVertexSize = usedVertexNr * topPointNr * axisNr;
	int axisSectionNr = geometricProperties->getAxisSectiontNr();
	sideVertexArray = new float[sideVertexSize];
	
	k = 0;
	for (int i = 0; i < axisNr; i++) {
		float heatFactor = (heatMap[radiusNr - 1][i] - environmentTemp) / (initTemp - environmentTemp);
		const tinycolormap::Color color = tinycolormap::GetColor(heatFactor, tinycolormap::ColormapType::Viridis);

		for (int j = 0; j < topPointNr; j++) {
			sideVertexArray[usedVertexNr * k + 0] = radiusLength * cos(j * sectionAngleRadian / topSectionNr);
			sideVertexArray[usedVertexNr * k + 1] = float((-axisLength / 2) + (axisLength / axisSectionNr) * i);
			sideVertexArray[usedVertexNr * k + 2] = -radiusLength * sin(j * sectionAngleRadian / topSectionNr);

			sideVertexArray[usedVertexNr * k + 3] = color.r();
			sideVertexArray[usedVertexNr * k + 4] = color.g();
			sideVertexArray[usedVertexNr * k + 5] = color.b();
			k++;
		}
	}

	//glGenBuffers(1, &sideVbo);
	//glBindBuffer(GL_ARRAY_BUFFER, sideVbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sideVertexSize, this->sideVertexArray, GL_STATIC_DRAW);
}

Snapshot::~Snapshot() {
	glDeleteBuffers(1, &vbo);
	if (vertexArray != nullptr) {
		delete[] vertexArray;
	}	

	glDeleteBuffers(1, &topVbo);
	if (topVertexArray != nullptr) {
		delete[] topVertexArray;
	}

	glDeleteBuffers(1, &sideVbo);
	if (sideVertexArray != nullptr) {
		delete[] sideVertexArray;
	}	
}