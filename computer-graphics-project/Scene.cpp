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

void Scene::resetCamera() {
	delete this->camera;
	createCamera();
}

void Scene::createCamera() {
	//TODO
	//Vector3f position = Vector3f(-1.3f, 0.5f, 0.6f);
	//Vector3f target = Vector3f(0.8f, -0.4f, -0.4f);
	//Vector3f position = Vector3f(2.53905f, 4.28667f, 2.92679f);
	//Vector3f target = Vector3f(0.0240572f, -0.976084f, 0.0122761f);
	//Vector3f up = Vector3f(-0.778f, -0.024f, -0.397f);
	Vector3f position = Vector3f(2.37184f, -0.914339f, 4.34706f);
	Vector3f target = Vector3f(0.901244f, -0.227319f, -0.257842f);
	Vector3f up = Vector3f(0.195412f, 0.838858f, -0.0559066f);
	this->camera = new Camera(position, target, up);
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
}