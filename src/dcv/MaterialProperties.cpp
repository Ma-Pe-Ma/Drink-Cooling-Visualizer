#include "MaterialProperties.h"

void MaterialProperties::update() {
	this->conductionConstant = 1000 * 1000 * this->heatConductivity / (this->density * this->heatcapacity);
	this->transferConstant = 1000 * this->heatTransferCoefficient / (this->density * this->heatcapacity);	
}