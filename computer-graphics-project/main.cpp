#include "Scene.h"
#include "Renderer.h"
#include "fstream"

int main(int argc, char **argv) {

	// Create our scene
	Scene *scene = new Scene();
	scene->backgroundColor = Vector3f(0.0f, 0.8f, 1.0f);

	scene->sunlight.direction = Vector3f(0.5f, -1.5f, -1.0f);
	//scene->sunlight.ambientColor = Vector3f(0.3f, 0.3f, 0.3f);
	//scene->sunlight.diffuseColor = Vector3f(0.7f, 0.7f, 0.7f);
	//scene->sunlight.specularColor = Vector3f(0.4f, 0.4f, 0.4f);
	scene->sunlight.ambientColor = Vector3f(0.5f, 0.5f, 0.5f);
	scene->sunlight.diffuseColor = Vector3f(0.1f, 0.1f, 0.1f);
	scene->sunlight.specularColor = Vector3f(0.1f, 0.1f, 0.1f);
	scene->sunlight.ambientIntensity = 1.f;
	scene->sunlight.diffuseIntensity = 1.f;
	scene->sunlight.specularIntensity = 1.f;

	scene->headlight.ambientColor = Vector3f(0.2f, 0.1f, 0.0f);
	scene->headlight.diffuseColor = Vector3f(0.5f, 0.4f, 0.2f);
	scene->headlight.specularColor = Vector3f(0.6f, 0.6f, 0.7f);
	scene->headlight.ambientIntensity = 1.f;
	scene->headlight.diffuseIntensity = 1.f;
	scene->headlight.specularIntensity = 1.f;
	scene->headlight.intensityKConst = 0.f;
	scene->headlight.intensityKLinear = 0.f;
	scene->headlight.intensitySquare = 0.5f;

	//Model* house = new Model("models\\kjottbasaren.obj", false, false);
	//house->scaling = 0.025f;
	//house->translation.set(0.0f, 0.0f, 0.0f);
	//scene->models.push_back(house);

	Model* ground = new Model("models\\ground.obj", false, false);
	ground->scaling = 0.5f;
	ground->translation.set(0.0f, -0.23f, 0.0f);
	//scene->models.push_back(ground);

	//Model* terrain = new Model("models\\terrain_mod.obj", false, false);
	Model* terrain = new Model("models\\terrain_mod.obj", true, false);
	terrain->scaling = 20.0f;
	//terrain->rotationX = Matrix4f::createRotation(90.0f, Vector3f(0.f, 1.f, 0.f));
	//terrain->scaling = 0.0000886328417f;
	//terrain->translation.set(-297873.469f, -336.940491f, -6700385.00f); //292811 //6694732
	//offset: -297873.469 -336.940491 -6700385.00
	//scaling factor: 8.86328417e-05
	scene->models.push_back(terrain);

	Model* skybox = new Model("models\\skybox.obj", false, false);
	skybox->scaling = 10.0f;
	skybox->translation.set(0.0f, -5.0f, 0.0f);
	scene->models.push_back(skybox);

	Renderer renderer = Renderer(scene);
	return renderer.render(argc, argv);
}