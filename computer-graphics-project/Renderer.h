#pragma once

#include <gl/glew.h>
#include <gl/glut.h>
#include <gl/GL.h>

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <unordered_map>

#include "Scene.h"

using namespace std;

class Renderer {

	// OpenGL callbacks
	void display();
	void idle();
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void motion(int, int);

	struct Texture {
		GLuint object = 0;				///< A texture object
		unsigned int width = 0;			///< The width of the current texture
		unsigned int height = 0;		///< The height of the current texture
		unsigned char *data = nullptr;	///< the array where the texture image will be stored
	};

	unordered_map<Model*, GLuint> vertexBufferObjects;
	unordered_map<Model*, GLuint> indexBufferObjects;
	unordered_map<string, Texture> textureObjects;

	struct Shader {
		string vertex;
		string fragment;
	};

	// Shaders
	Shader perFragmentIlluminationShader = { "shaders/per_fragment_illumination.v.glsl", "shaders/per_fragment_illumination.f.glsl" };
	Shader shader = perFragmentIlluminationShader;

	// Shaders
	GLuint ShaderProgram = 0;	// Shader program
	GLint ShadingEffectLocation = -1;
	GLint CameraTransformationLocation = -1; // Reference to the model-view matrix uniform variable
	GLint ModelTransformationLocation = -1;
	GLint ModelNormalsTransformationLocation = -1;
	GLint SamplerLocation = -1;	// Reference to the texture sampler uniform variable
	//GLint TimeLocation = -1;	// Reference to the time uniform variable

	// Locations for shader variables
	GLint CameraPositionLoc = -1;
	GLint DLightDirLoc = -1;
	GLint DLightAColorLoc = -1;
	GLint DLightDColorLoc = -1;
	GLint DLightSColorLoc = -1;
	GLint DLightAIntensityLoc = -1;
	GLint DLightDIntensityLoc = -1;
	GLint DLightSIntensityLoc = -1;
	GLint PLightDirLoc = -1;
	GLint PLightAColorLoc = -1;
	GLint PLightDColorLoc = -1;
	GLint PLightSColorLoc = -1;
	GLint PLightAIntensityLoc = -1;
	GLint PLightDIntensityKConstLoc = -1;
	GLint PLightDIntensityKLinearLoc = -1;
	GLint PLightDIntensityKSquareLoc = -1;
	GLint PLightDIntensityLoc = -1;
	GLint PLightSIntensityLoc = -1;
	GLint MaterialAColorLoc = -1;
	GLint MaterialDColorLoc = -1;
	GLint MaterialSColorLoc = -1;
	GLint MaterialShineLoc = -1;
	GLint MaterialTextureIntensityLoc = -1;

	// Mouse interaction
	int MouseX, MouseY;		// The last position of the mouse
	int MouseButton;		// The last mouse button pressed or released

	// Clock
	clock_t Timer;

	// Shading effect
	int shadingEffect = 0;

	// Scene
	Scene* scene;
public:
	Renderer(Scene*);
	~Renderer();
	int render(int, char**);

	static void displayWrapper();
	static void idleWrapper();
	static void keyboardWrapper(unsigned char, int, int);
	static void mouseWrapper(int, int, int, int);
	static void motionWrapper(int, int);
private:
	static Renderer *instance;
	void initMesh();
	bool initShaders();
	string readTextFile(const string&);
};