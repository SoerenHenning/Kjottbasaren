#include "Renderer.h"
#include "lodepng.h"


Renderer *Renderer::instance = NULL;

Renderer::Renderer(Scene* scene) {
	this->scene = scene;
	Timer = clock();
}

Renderer::~Renderer() {
}

int Renderer::render(int argc, char **argv) {
	if (Renderer::instance != NULL) {
		// Renderer already set
		return -1;
	}
	Renderer::instance = this;
	
	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("Computer Graphics Project");

	// Initialize OpenGL callbacks
	glutDisplayFunc(displayWrapper);
	glutIdleFunc(idleWrapper);
	glutKeyboardFunc(keyboardWrapper);
	glutMouseFunc(mouseWrapper);
	glutMotionFunc(motionWrapper);

	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cerr << "Error initializing glew: \n"
			<< reinterpret_cast<const char*>(glewGetErrorString(res)) << endl;
		return -1;
	}

	// Initialize program variables
	// OpenGL
	glClearColor(scene->backgroundColor.x(), scene->backgroundColor.y(), scene->backgroundColor.z(), 0.0f); // background color
	glEnable(GL_DEPTH_TEST);	        // enable depth ordering
	glEnable(GL_CULL_FACE);		        // enable back-face culling
	glFrontFace(GL_CCW);		        // vertex order for the front face
	glCullFace(GL_BACK);		        // back-faces should be removed

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // draw polygons as wireframe
	glPolygonMode(GL_FRONT, GL_FILL);   // draw polygons as solid

	// Initialize program variables

	// Mesh
	initMesh();
	
	// Shaders
	if (!initShaders()) {
		cout << "Press Enter to exit..." << endl;
		getchar();
		return -1;
	}
	
	// Start the main event loop
	glutMainLoop();

	return 0;
}

void Renderer::display() {
	
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);
	
	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	scene->getCamera()->setAspectRatio((1.0f * width) / height);
	Matrix4f cameraTransformation = scene->getCamera()->getTransformationMatrix();
	glUniformMatrix4fv(TrLocation, 1, GL_FALSE, cameraTransformation.get());

	Matrix4f worldTransformation = scene->worldRotation;
	glUniformMatrix4fv(WorldTransformationLocation, 1, GL_FALSE, worldTransformation.get());

	glUniform3fv(CameraPositionLoc, 1, scene->getCamera()->getPosition().get());

	// Set the sunlight's parameters
	glUniform3f(DLightDirLoc, scene->sunlight.direction.x(), scene->sunlight.direction.y(), scene->sunlight.direction.z());
	glUniform3f(DLightAColorLoc, scene->sunlight.ambientColor.x(), scene->sunlight.ambientColor.y(), scene->sunlight.ambientColor.z());
	glUniform3f(DLightDColorLoc, scene->sunlight.diffuseColor.x(), scene->sunlight.diffuseColor.y(), scene->sunlight.diffuseColor.z());
	glUniform3f(DLightSColorLoc, scene->sunlight.specularColor.x(), scene->sunlight.specularColor.y(), scene->sunlight.specularColor.z());
	glUniform1f(DLightAIntensityLoc, scene->sunlight.ambientIntensity);
	glUniform1f(DLightDIntensityLoc, scene->sunlight.diffuseIntensity);
	glUniform1f(DLightSIntensityLoc, scene->sunlight.specularIntensity);

	// Set the headlight's parameters
	glUniform3f(PLightAColorLoc, scene->headlight.ambientColor.x(), scene->headlight.ambientColor.y(), scene->headlight.ambientColor.z());
	glUniform3f(PLightDColorLoc, scene->headlight.diffuseColor.x(), scene->headlight.diffuseColor.y(), scene->headlight.diffuseColor.z());
	glUniform3f(PLightSColorLoc, scene->headlight.specularColor.x(), scene->headlight.specularColor.y(), scene->headlight.specularColor.z());
	glUniform1f(PLightAIntensityLoc, scene->headlight.ambientIntensity);
	glUniform1f(PLightDIntensityLoc, scene->headlight.diffuseIntensity);
	glUniform1f(PLightSIntensityLoc, scene->headlight.specularIntensity);
	glUniform1f(PLightDIntensityKConstLoc, scene->headlight.intensityKConst);
	glUniform1f(PLightDIntensityKLinearLoc, scene->headlight.intensityKLinear);
	glUniform1f(PLightDIntensityKSquareLoc, scene->headlight.intensitySquare);

	// Enable the vertex attributes
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //tex_coords
	glEnableVertexAttribArray(2); //normals

	//TODO this block is temp
	// Set the uniform variable for the texture unit (texture unit 0)
// 	glUniform1i(SamplerLocation, 0); //TODO necessary?
	/* See here for nicer code:
	GLint texLoc = glGetAttribLocation(ShaderProgram, "tex_coords");
	glEnableVertexAttribArray(texLoc);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(3 * sizeof(float)));
	*/



	// Draw the models
	for (auto const& model : scene->models) {

		GLuint vertexBufferObject = vertexBufferObjects[model];
		GLuint indexBufferObject = indexBufferObjects[model];

		// Bind the buffers
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

		// Set the vertex attributes format
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(3 * sizeof(float)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>((3 * sizeof(float)) + (2 * sizeof(float))));

		for (int i = 0; i < model->modelObj.getNumberOfMeshes(); ++i) {
			const ModelOBJ::Mesh *mesh = &model->modelObj.getMesh(i);
			const ModelOBJ::Material *material = mesh->pMaterial;

			glUniform1i(SamplerLocation, 0); //TODO necessary?

			glUniform3f(MaterialAColorLoc, material->ambient[0], material->ambient[1], material->ambient[2]);
			glUniform3f(MaterialDColorLoc, material->diffuse[0], material->diffuse[1], material->diffuse[2]);
			glUniform3f(MaterialSColorLoc, material->specular[0], material->specular[1], material->specular[2]);
			glUniform1f(MaterialShineLoc, material->shininess * 128.0f);
		
			string key = material->name;
			if (textureObjects.count(key) == 1) {
				Texture texture = textureObjects[key];
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(MaterialTextureLoc, true);
				glEnable(GL_TEXTURE_2D); //TODO required?
				glBindTexture(GL_TEXTURE_2D, texture.object); //TODO set correct texture here
			} else {
				glDisable(GL_TEXTURE_2D); //TODO required?
				glUniform1i(MaterialTextureLoc, false);
			}

			// Draw the elements on the GPU
			glDrawElements(GL_TRIANGLES, mesh->triangleCount * 3, GL_UNSIGNED_INT, (void*) (mesh->startIndex * sizeof(GLuint)));
		}

		
	}

	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

void Renderer::idle() {
	clock_t now = clock();
	if (scene->rotating) {
		scene->worldRotation *= Matrix4f::createRotation(10.f * (now - Timer) / CLOCKS_PER_SEC, Vector3f(0.f, 1.f, 0.f));
	}
	Timer = now;
	glutPostRedisplay();
}

void Renderer::keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 'r': // Reset camera status
		scene->resetCamera();
		break;
	case 'w':
		scene->getCamera()->moveForward(0.1f);
		break;
	case 'a':
		scene->getCamera()->moveRight(-0.1f);
		break;
	case 's':
		scene->getCamera()->moveForward(-0.1f);
		break;
	case 'd':
		scene->getCamera()->moveRight(0.1f);
		break;
	case 'c':
		scene->getCamera()->moveUp(-0.1f);
		break;
	case ' ':
		scene->getCamera()->moveUp(0.1f);
		break;
	case 'n':	// Increase field of view
		scene->getCamera()->increaseFieldOfView(1.f);
		break;
	case 'm':	// Decrease field of view
		scene->getCamera()->increaseFieldOfView(-1.f);
		break;
	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'q':  // terminate the application
		exit(0);
		break;
	case 'b':  // switch perspectiv to orth
		scene->getCamera()->toggleProjection();
		break;
	case 'v':  // toggle rotation
		scene->rotating = !scene->rotating;
		break;
	case 'i':  // print info about camera
		scene->getCamera()->printStatus();
		break;
	case 'p': // change to wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'o': // change to polygon rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'y': // Toggle Sunlight
		scene->sunlight.ambientIntensity = (scene->sunlight.ambientIntensity == 0.f) ? 1.f : 0.f;
		scene->sunlight.diffuseIntensity = (scene->sunlight.diffuseIntensity == 0.f) ? 1.f : 0.f;
		scene->sunlight.specularIntensity = (scene->sunlight.specularIntensity == 0.f) ? 1.f : 0.f;
		break;
	case 'x': // Toggle Headlight
		scene->headlight.ambientIntensity = (scene->headlight.ambientIntensity == 0.f) ? 1.f : 0.f;
		scene->headlight.diffuseIntensity = (scene->headlight.diffuseIntensity == 0.f) ? 1.f : 0.f;
		scene->headlight.specularIntensity = (scene->headlight.specularIntensity == 0.f) ? 1.f : 0.f;
		break;
	case '1':
		cout << "Switch to per vertex illumination shaders..." << endl;
		shader = perVertexIlluminationShader;
		if (initShaders()) {
			cout << "> done." << endl;
		}
		break;
	case '2':
		cout << "Switch to per fragment illumination shaders..." << endl;
		shader = perFragmentIlluminationShader;
		if (initShaders()) {
			cout << "> done." << endl;
		}
		break;
	case '3':
		cout << "Switch to black and white per vertex illumination shaders..." << endl;
		shader = perVertexIlluminationBWShader;
		if (initShaders()) {
			cout << "> done." << endl;
		}
		break;
	case 'l':
		cout << "Re-loading shaders..." << endl;
		if (initShaders()) {
			cout << "> done." << endl;
		}
	}
	glutPostRedisplay();
}

void Renderer::mouse(int button, int state, int x, int y) {
	// Store the current mouse status
	MouseButton = button;
	MouseX = x;
	MouseY = y;
}

void Renderer::motion(int x, int y) {
	if (MouseButton == GLUT_RIGHT_BUTTON) {
		scene->getCamera()->moveRight(0.005f * (MouseX - x));
		scene->getCamera()->moveForward(0.005f * (y - MouseY));

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		scene->getCamera()->zoomIn(0.003f * (y - MouseY));

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		scene->getCamera()->rotateHorizontal(0.2f * (MouseX - x));
		scene->getCamera()->rotateVertical(0.2f * (MouseY - y));

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}

	glutPostRedisplay(); // Specify that the scene needs to be updated
}

void Renderer::initMesh() {
	for (auto const& model : scene->models) {
		// VBO
		GLuint vertexBufferObject = 0;
		glGenBuffers(1, &vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, model->modelObj.getNumberOfVertices() * sizeof(ModelOBJ::Vertex), model->modelObj.getVertexBuffer(), GL_STATIC_DRAW);
		vertexBufferObjects.emplace(model, vertexBufferObject);

		// IBO
		GLuint indexBufferObject = 0;
		glGenBuffers(1, &indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * model->modelObj.getNumberOfTriangles() * sizeof(int), model->modelObj.getIndexBuffer(), GL_STATIC_DRAW);
		indexBufferObjects.emplace(model, indexBufferObject);

		// Check the materials for the texture
		for (int i = 0; i < model->modelObj.getNumberOfMaterials(); ++i) {
			const ModelOBJ::Material material = model->modelObj.getMaterial(i);

			// if the current material has a texture
			if (material.colorMapFilename != "") {
				Texture texture;
				GLuint textureObject = 0; //TODO

				// Load the texture
				if (texture.data != nullptr) {
					free(texture.data);
				}
				unsigned int fail = lodepng_decode_file(&(texture.data), &(texture.width), &(texture.height),
					("models\\" + material.colorMapFilename).c_str(),
					LCT_RGB, 8); // Remember to check the last 2 parameters
				if (fail != 0) {
					cerr << "Error: cannot load texture file " << material.colorMapFilename << endl;
					//return false;
				}

				// Create the texture object
				if (texture.object != 0) {
					glDeleteTextures(1, &(texture.object));
				}
				glGenTextures(1, &(texture.object));

				// Bind it as a 2D texture (note that other types of textures are supported as well)
				glBindTexture(GL_TEXTURE_2D, texture.object);

				// Set the texture data
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGB,			// remember to check this
					texture.width,
					texture.height,
					0,
					GL_RGB,			// remember to check this
					GL_UNSIGNED_BYTE,
					texture.data);

				//TODO try to remove from from loop
				// Configure texture parameter
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


				textureObjects.emplace(material.name, texture);
			}
		}
		
	}
	


	/*
	const ModelOBJ::Vertex *vb = Model.getVertexBuffer();

	for (int i = 0; i < Model.getNumberOfVertices(); i++) {
	cout << "vb[" << i << "].position: (" << vb[i].position[0] << "," << vb[i].position[1] << "," << vb[i].position[2] << ")" << endl;
	}

	for (int i = 0; i < Model.getNumberOfVertices(); i++) {
	cout << "vb[" << i << "].normal: (" << vb[i].normal[0] << "," << vb[i].normal[1] << "," << vb[i].normal[2] << ")" << endl;
	}

	for (int i = 0; i < Model.getNumberOfVertices(); i++) {
	cout << "vb[" << i << "].texCoord: (" << vb[i].texCoord[0] << "," << vb[i].texCoord[1] << ")" << endl;
	}
	*/
}

/*
// initTextures()
bool initTextures() {
	// Load the texture image
	if (TextureData != nullptr)
		free(TextureData);
	unsigned int fail = lodepng_decode_file(&TextureData, &TextureWidth, &TextureHeight,
		"texture.png", LCT_RGBA, 8);
	if (fail != 0) {
		cerr << "Error: cannot load the texture file. " << endl;
		return false;
	}

	// Create the texture object
	if (TextureObject != 0)
		glDeleteTextures(1, &TextureObject);
	glGenTextures(1, &TextureObject);

	// Bind it as a 2D texture (note that other types of textures are supported as well)
	glBindTexture(GL_TEXTURE_2D, TextureObject);

	// Set the texture data
	glTexImage2D(
		GL_TEXTURE_2D,	// type of texture
		0,				// level of detail (used for mip-mapping only)
		GL_RGBA,		// color components (how the data should be interpreted)
		TextureWidth,	// texture width (must be a power of 2 on some systems)
		TextureHeight,	// texture height (must be a power of 2 on some systems)
		0,				// border thickness (just set this to 0)
		GL_RGBA,		// data format (how the data is supplied)
		GL_UNSIGNED_BYTE, // the basic type of the data array
		TextureData		// pointer to the data
	);

	// Configure texture parameter
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}
*/

bool Renderer::initShaders() {
	// Create the shader program and check for errors
	if (ShaderProgram != 0)
		glDeleteProgram(ShaderProgram);
	ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0) {
		cerr << "Error: cannot create shader program." << endl;
		return false;
	}

	// Create the shader objects and check for errors
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (vertexShader == 0 || fragmentShader == 0) {
		cerr << "Error: cannot create shader objects." << endl;
		return false;
	}

	// Read and set the source code for the vertex shader
	string vertexShaderText = readTextFile(shader.vertex);
	const char* vertexShaderCode = vertexShaderText.c_str();
	int vertexShaderLength = static_cast<int>(vertexShaderText.length());
	if (vertexShaderLength == 0)
		return false;
	glShaderSource(vertexShader, 1, &vertexShaderCode, &vertexShaderLength);

	// Read and set the source code for the fragment shader
	string fragmentShaderText = readTextFile(shader.fragment);
	const char *fragmentShaderCode = fragmentShaderText.c_str();
	int fragmentShaderLength = static_cast<int>(fragmentShaderText.length());
	if (fragmentShaderLength == 0)
		return false;
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, &fragmentShaderLength);

	// Compile the shaders
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Check for compilation error
	GLint success;
	GLchar errorLog[1024];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile vertex shader.\nError log:\n" << errorLog << endl;
		return false;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile fragment shader.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Attach the shader to the program and link it
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);

	// Check for linking error
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(ShaderProgram);

	// Check for validation error
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot validate shader program.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Get the location of the uniform variables
	TrLocation = glGetUniformLocation(ShaderProgram, "transformation");
	WorldTransformationLocation = glGetUniformLocation(ShaderProgram, "world_transformation");
	SamplerLocation = glGetUniformLocation(ShaderProgram, "transformation");
	//TimeLocation = glGetUniformLocation(ShaderProgram, "time");
	//assert(TrLocation != -1 && SamplerLocation != -1 && TimeLocation != -1);
	//assert(TrLocation != -1 && SamplerLocation != -1);

	CameraPositionLoc = glGetUniformLocation(ShaderProgram, "camera_position");

	DLightDirLoc = glGetUniformLocation(ShaderProgram, "d_light_direction");
	DLightAColorLoc = glGetUniformLocation(ShaderProgram, "d_light_a_color");
	DLightDColorLoc = glGetUniformLocation(ShaderProgram, "d_light_d_color");
	DLightSColorLoc = glGetUniformLocation(ShaderProgram, "d_light_s_color");
	DLightAIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_a_intensity");
	DLightDIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_d_intensity");
	DLightSIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_s_intensity");

	PLightDirLoc = glGetUniformLocation(ShaderProgram, "p_light_direction");
	PLightAColorLoc = glGetUniformLocation(ShaderProgram, "p_light_a_color");
	PLightDColorLoc = glGetUniformLocation(ShaderProgram, "p_light_d_color");
	PLightSColorLoc = glGetUniformLocation(ShaderProgram, "p_light_s_color");
	PLightAIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_a_intensity");
	PLightDIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_d_intensity");
	PLightDIntensityKConstLoc = glGetUniformLocation(ShaderProgram, "p_light_d_intensity_k_const");
	PLightDIntensityKLinearLoc = glGetUniformLocation(ShaderProgram, "p_light_d_intensity_k_linear");
	PLightDIntensityKSquareLoc = glGetUniformLocation(ShaderProgram, "p_light_d_intensity_k_square");
	PLightSIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_s_intensity");

	MaterialAColorLoc = glGetUniformLocation(ShaderProgram, "material_a_color");
	MaterialDColorLoc = glGetUniformLocation(ShaderProgram, "material_d_color");
	MaterialSColorLoc = glGetUniformLocation(ShaderProgram, "material_s_color");
	MaterialShineLoc = glGetUniformLocation(ShaderProgram, "material_shininess");
	MaterialTextureLoc = glGetUniformLocation(ShaderProgram, "material_texture");

	// Shaders can be deleted now
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

string Renderer::readTextFile(const string& pathAndFileName) {
	// Try to open the file
	ifstream fileIn(pathAndFileName);
	if (!fileIn.is_open()) {
		cerr << "Error: cannot open file " << pathAndFileName.c_str();
		return "";
	}

	// Read the file
	string text = "";
	string line;
	while (!fileIn.eof()) {
		getline(fileIn, line);
		text += line + "\n";
		bool bad = fileIn.bad();
		bool fail = fileIn.fail();
		if (fileIn.bad() || (fileIn.fail() && !fileIn.eof())) {
			cerr << "Warning: problems reading file " << pathAndFileName.c_str()
				<< "\nBad flag: " << bad << "\tFail flag: " << fail
				<< "\nText read: \n" << text.c_str();
			fileIn.close();
			return text;
		}
	}
	// finalize
	fileIn.close();

	return text;
}

void Renderer::displayWrapper() {
	instance->display();
}

void Renderer::idleWrapper() {
	instance->idle();
}

void Renderer::keyboardWrapper(unsigned char key, int x, int y) {
	instance->keyboard(key, x, y);
}

void Renderer::mouseWrapper(int button, int state, int x, int y) {
	instance->mouse(button, state, x, y);
}

void Renderer::motionWrapper(int x, int y) {
	instance->motion(x, y);
}
