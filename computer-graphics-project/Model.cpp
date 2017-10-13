#include "Model.h"

Model::Model(std::string filename, bool normalize, bool reverseWinding) {
	// Load the OBJ model
	if (!modelObj.import(filename.c_str())) {
		throw std::invalid_argument("Cannot load model.");
	}
	if (normalize) {
		modelObj.normalize();
	}
	if (reverseWinding) {
		modelObj.reverseWinding();
	}
}

Model::~Model() {
}