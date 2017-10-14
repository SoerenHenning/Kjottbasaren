#include "Scene.h"

Scene::Scene() {
	createCamera();
}

Scene::~Scene(){
	delete this->camera;
}

void Scene::resetCamera() {
	delete this->camera;
	createCamera();
}

void Scene::createCamera() {
	Vector3f position = Vector3f(-1.3f, 0.5f, 0.6f);
	Vector3f target = Vector3f(0.8f, -0.4f, -0.4f);
	this->camera = new Camera(position, target);
}