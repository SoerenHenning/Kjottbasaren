#pragma once

#include "Vector3.h"
#include "Model.h"
#include "Camera.h"

struct Scene {
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

	Camera* camera;
	int currentCameraId = 0; //TODO remove
	std::vector<Camera*> cameras; //TODO remove
	std::vector<Model*> models;
	Vector3f backgroundColor;
	Matrix4f worldRotation; //TODO remove
	bool rotating = true; // camera movement
	float textureIntensity;
	DirectionalLight sunlight;
	PointLight headlight;
	public:
		Scene();
		~Scene();
		void resetCamera();
		void setCamera(int cameraId);
		void toggleCameras();
		Camera* getCamera();
	private:
			void createCamera();
};