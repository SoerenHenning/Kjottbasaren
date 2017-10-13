#include "Scene.h"

Scene::Scene() {
	resetCamera();
}

Scene::~Scene(){
}

void Scene::resetCamera() {
	delete this->camera;
	Vector3f position = Vector3f(-1.3f, 0.5f, 0.6f);
	Vector3f target = Vector3f(0.8f, -0.4f, -0.4f);
	this->camera = new Camera(position, target);
}
