#include "Scene.h"

Scene::Scene() {
	createCamera();
	worldRotation.identity();
	rotating = false;
	textureIntensity = 1.0;
}

Scene::~Scene(){
	delete this->camera;
}

//TODO rename to MainCamra
void Scene::resetCamera() {
	delete this->camera;
	createCamera();
	//TODO do this for this->cameras.at(0)
}

//TODO rename to MainCamra
void Scene::createCamera() {
	//TODO
	//Vector3f position = Vector3f(-1.3f, 0.5f, 0.6f);
	//Vector3f target = Vector3f(0.8f, -0.4f, -0.4f);
	Vector3f position = Vector3f(2.53905f, 4.28667f, 2.92679f);
	Vector3f target = Vector3f(0.0240572f, -0.976084f, 0.0122761f);
	Vector3f up = Vector3f(-0.778f, -0.024f, -0.397f);
	//Position: (2.53905, 4.28667, 2.92679)
	//Target : (0.0240572, -0.976084, 0.0122761)
	//Vector3f target = Vector3f(0.0f, -0.1f, 0.0f);
	//Position: (-1.16493, 30.9256, 4.65228)
	//Target : (0.000460496, -0.958302, -0.112545)
	//Up : (0.00351366, 0.100665, -0.857123)
	//Vector3f position = Vector3f(-1000.3f, 1000.5f, 1000.6f);
	//Vector3f target = Vector3f(0.8f, -0.4f, -0.4f);
	this->camera = new Camera(position, target, up);
	//TODO do this for this->cameras.at(0)
}

void Scene::setCamera(int cameraId) {
	if (cameraId < this->cameras.size()) {
		this->currentCameraId = cameraId;
	}
}

void Scene::toggleCameras() {
	this->currentCameraId = (this->currentCameraId + 1) % this->cameras.size();
}

Camera* Scene::getCamera() {
	return this->camera;
	//return this->cameras.at(this->currentCameraId);
}