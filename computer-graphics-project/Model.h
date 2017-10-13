#include <string>

#include "ModelObj.h"
#include "Vector3.h"

// Wrapper struct for ModelOBJ
struct Model {
	ModelOBJ modelObj;
	Vector3f materialAmbientColor;
	Vector3f materialDiffuseColor;
	Vector3f materialSpecularColor;
	float materialShininess;
public:
	Model(std::string, bool);
	~Model();
};
