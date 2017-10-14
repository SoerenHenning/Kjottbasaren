#include "Renderer.h"

Renderer *Renderer::instance = NULL;

Renderer::Renderer(Scene* scene) {
	this->scene = scene;
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

	scene->camera->setAspectRatio((1.0f * width) / height);
	Matrix4f cameraTransformation = scene->camera->getTransformationMatrix();
	glUniformMatrix4fv(TrLocation, 1, GL_FALSE, cameraTransformation.get());

	
	glUniform3fv(CameraPositionLoc, 1, scene->camera->getPosition().get());

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
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	

	// Draw the models
	for (auto const& model : scene->models) {
		GLuint vertexBufferObject = vertexBufferObjects[model];
		GLuint indexBufferObject = indexBufferObjects[model];

		// Set the material parameters for the ground
		glUniform3f(MaterialAColorLoc, model->materialAmbientColor.x(), model->materialAmbientColor.y(), model->materialAmbientColor.z());
		glUniform3f(MaterialDColorLoc, model->materialDiffuseColor.x(), model->materialDiffuseColor.y(), model->materialDiffuseColor.z());
		glUniform3f(MaterialSColorLoc, model->materialSpecularColor.x(), model->materialSpecularColor.y(), model->materialSpecularColor.z());
		glUniform1f(MaterialShineLoc, model->materialShininess);

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
			sizeof(ModelOBJ::Vertex), reinterpret_cast<const GLvoid*>((3 * sizeof(float)) + (2 * sizeof(float))));

		// Draw the elements on the GPU
		glDrawElements(GL_TRIANGLES, model->modelObj.getNumberOfIndices(), GL_UNSIGNED_INT, 0);
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
}

void Renderer::keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 'r': // Reset camera status
		scene->resetCamera();
		break;
	case 'w':
		scene->camera->moveForward(0.1f);
		break;
	case 'a':
		scene->camera->moveRight(-0.1f);
		break;
	case 's':
		scene->camera->moveForward(-0.1f);
		break;
	case 'd':
		scene->camera->moveRight(0.1f);
		break;
	case 'c':
		scene->camera->moveUp(-0.1f);
		break;
	case ' ':
		scene->camera->moveUp(0.1f);
		break;
	case 'n':	// Increase field of view
		scene->camera->increaseFieldOfView(1.f);
		break;
	case 'm':	// Decrease field of view
		scene->camera->increaseFieldOfView(-1.f);
		break;
	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'q':  // terminate the application
		exit(0);
		break;
	case 'b':  // switch perspectiv to orth
		scene->camera->toggleProjection();
		break;
	case 'i':  // print info about camera
		scene->camera->printStatus();
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
		scene->camera->moveRight(0.005f * (MouseX - x));
		scene->camera->moveForward(0.005f * (y - MouseY));

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		scene->camera->zoomIn(0.003f * (y - MouseY));

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		scene->camera->rotateHorizontal(0.2f * (MouseX - x));
		scene->camera->rotateVertical(0.2f * (MouseY - y));

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
	//SamplerLocation = glGetUniformLocation(ShaderProgram, "sampler");
	TimeLocation = glGetUniformLocation(ShaderProgram, "time");
	//assert(TrLocation != -1 && SamplerLocation != -1 && TimeLocation != -1);
	assert(TrLocation != -1);

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
