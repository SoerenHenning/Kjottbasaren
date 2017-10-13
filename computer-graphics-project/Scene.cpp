#include "Scene.h"

Model::Model(string filename, bool normalize) {
	// Load the OBJ model
	if (!modelObj.import(filename.c_str())) {
		throw std::invalid_argument("Cannot load model.");
	}
	if (normalize) {
		modelObj.normalize();
	}
}

Model::~Model() {
}
