#pragma once

#include <string>

#include "ModelObj.h"
#include "Vector3.h"

// Wrapper struct for ModelOBJ
struct Model {
	ModelOBJ modelObj;
public:
	Model(std::string, bool, bool);
	~Model();
};
