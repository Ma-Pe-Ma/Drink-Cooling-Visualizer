#ifndef MATERIALPROPERTIES_H
#define MATERIALPROPERTIES_H


class MaterialProperties {
	//material properties
	float heatcapacity;
	float density;
	float heatConductivity;
	float heatTransferCoefficient;

	float conductionConstant;
	float transferConstant;

public:
	MaterialProperties(float heatcapacity, float density, float heatConductivity, float heatTransferCoefficient) {
		this->heatcapacity = heatcapacity;
		this->density = density;
		this->heatConductivity = heatConductivity;
		this->heatTransferCoefficient = heatTransferCoefficient;
		update();
	}

	float* getHeatcapacityPointer() {
		return &heatcapacity;
	}

	float* getDensityPointer() {
		return &density;
	}

	float* getHeatConductivityPointer() {
		return &heatConductivity;
	}

	float* getHeatTransferCoefficientPointer() {
		return &heatTransferCoefficient;
	}

	float getTransferConstant() {
		return transferConstant;
	}

	float getConductionConstant() {
		return conductionConstant;
	}
	void update();
};
#endif 
