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
	rotationX.identity();
	rotationY.identity();
	translation.set(0.0f, 0.0f, 0.0f);
	scaling = 1.0f;
}

Model::~Model() {
}

Matrix4f Model::getTransformation() {
	Matrix4f ref = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	return Matrix4f::createTranslation(translation) * rotationX * rotationY * Matrix4f::createScaling(scaling, scaling, scaling);
}
