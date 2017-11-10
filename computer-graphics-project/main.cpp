#include "Scene.h"
#include "Renderer.h"

int main(int argc, char **argv) {
	// Create our scene
	Scene *scene = new Scene();
	scene->backgroundColor = Vector3f(0.0f, 0.8f, 1.0f);

	scene->sunlight.direction = Vector3f(0.5f, -1.5f, -1.0f);
	scene->sunlight.ambientColor = Vector3f(0.3f, 0.3f, 0.3f);
	scene->sunlight.diffuseColor = Vector3f(0.7f, 0.7f, 0.7f);
	scene->sunlight.specularColor = Vector3f(0.4f, 0.4f, 0.4f);
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

	Model* house = new Model("models\\kjottbasaren.obj", false, false);
	house->scaling = 0.025;
	house->translation.set(0.0f, 0.0f, 0.0f);
	scene->models.push_back(house);

	//Model* tree = new Model("models\\Tree1.obj", false, true);
	//scene->models.push_back(tree);

	//Model* capsule = new Model("models\\capsule.obj", false, true);
	//scene->models.push_back(capsule);

	//Model* cottage = new Model("models\\Snow covered CottageOBJ.obj", false, false);
	//cottage->scaling = 0.005;
	//cottage->translation.set(3.0f, 0.0f, 3.0f);
	//scene->models.push_back(cottage);

	//Model* ground = new Model("models\\plane.obj", false, false);
	//scene->models.push_back(ground);

	Model* ground = new Model("models\\ground.obj", false, false);
	ground->scaling = 0.5;
	ground->translation.set(0.0f, -0.23f, 0.0f);
	scene->models.push_back(ground);

	Renderer renderer = Renderer(scene);
	return renderer.render(argc, argv);
}