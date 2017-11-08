#include "Scene.h"

Scene::Scene() {
	createCamera();
	worldRotation.identity();
	rotating = false;
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
	Vector3f position = Vector3f(-1.3f, 0.5f, 0.6f);
	Vector3f target = Vector3f(0.8f, -0.4f, -0.4f);
	this->camera = new Camera(position, target);
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