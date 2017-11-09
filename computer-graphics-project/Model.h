#pragma once

#include <string>

#include "ModelObj.h"
#include "Vector3.h"
#include "Matrix4.h"

// Wrapper struct for ModelOBJ
struct Model {
	ModelOBJ modelObj;
	Matrix4f rotationX, rotationY;
	Vector3f translation;
	float scaling;
public:
	Model(std::string, bool, bool);
	~Model();
	Matrix4f getTransformation();
};
