#include "ModelObj.h"
#include "Vector3.h"
#include "Camera.h"

struct Model {
	ModelOBJ modelObj;
	Vector3f materialAmbientColor;
	Vector3f materialDiffuseColor;
	Vector3f materialSpecularColor;
	float materialShininess;
	public:
		Model(string, bool);
		~Model();
};

struct DirectionalLight {
	Vector3f direction;
	Vector3f ambientColor;
	Vector3f diffuseColor;
	Vector3f specularColor;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};

struct PointLight {
	Vector3f position;
	Vector3f ambientColor;
	Vector3f diffuseColor;
	Vector3f specularColor;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float intensityKConst;
	float intensityKLinear;
	float intensitySquare;
};

struct Scene {
	Camera* camera;
	std::vector<Model*> models;
	Vector3f backgroundColor;
	DirectionalLight sunlight;
	PointLight headlight;
	public:
		Scene();
		~Scene();
		void resetCamera();
};