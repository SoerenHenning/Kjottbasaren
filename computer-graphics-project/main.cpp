#include <gl/glew.h>
#include <gl/glut.h>
#include <gl/GL.h>

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "Vector3.h"
#include "Matrix4.h"
#include "ModelObj.h"

using namespace std;

// TODO Move to own class
/// A simple structure to handle a moving camera (perspective projection)
struct Camera {
	Vector3f position;	///< the position of the camera
	Vector3f target;	///< the direction the camera is looking at
	Vector3f up;		///< the up vector of the camera

	float fov;			///< camera field of view
	float ar;			///< camera aspect ratio

	float zNear, zFar;	///< depth of the near and far plane

	float zoom;			///< an additional scaling parameter
};


// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void idle();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);

// --- Other methods ------------------------------------------------------------------------------
bool initMesh(ModelOBJ&, string, bool, GLuint&, GLuint&);
bool initShaders();
Matrix4f computeCameraTransform(const Camera&);
void initCamera(Camera&);
string readTextFile(const string&);


// --- Global variables ---------------------------------------------------------------------------
// 3D models
// House
ModelOBJ houseModel;		///< A 3D model
string houseModelFilename = "capsule\\FinalBuilding.obj";
GLuint houseVBO = 0;		///< A vertex buffer object
GLuint houseIBO = 0;		///< An index buffer object
// Ground
ModelOBJ groundModel;		///< A 3D model
string groundModelFilename = "capsule\\plane.obj";
GLuint groundVBO = 0;		///< A vertex buffer object
GLuint groundIBO = 0;		///< An index buffer object


// Shaders
GLuint ShaderProgram = 0;	///< A shader program
GLint TrLocation = -1;		///< Reference to the model-view matrix uniform variable
//GLint SamplerLocation = -1;	///< Reference to the texture sampler uniform variable
GLint TimeLocation = -1;	///< Reference to the time uniform variable

// Colors, copied from Tutorial 6
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

// Mouse interaction
int MouseX, MouseY;		///< The last position of the mouse
int MouseButton;		///< The last mouse button pressed or released


// Camera
Camera Cam;
int prjType = 0; // Projection type, 0 is perspective, 1 is orth

// --- main() -------------------------------------------------------------------------------------
/// The entry point of the application
int main(int argc, char **argv) {

	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("Computer Graphics Project");

	// Initialize OpenGL callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	
	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cerr << "Error initializing glew: \n" 
			<< reinterpret_cast<const char*>(glewGetErrorString(res)) << endl;
		return -1;
	}

	// Initialize program variables
	// OpenGL
	glClearColor(0.0f, 0.8f, 1.0f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);	        // enable depth ordering
    //glEnable(GL_CULL_FACE);		        // enable back-face culling //TODO disabled temp
    glFrontFace(GL_CCW);		        // vertex order for the front face
	glCullFace(GL_BACK);		        // back-faces should be removed

	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // draw polygons as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);   // draw polygons as solid
		

	// Initialize program variables
	// Camera 
	initCamera(Cam);

	// Mesh
	initMesh(houseModel, houseModelFilename, true, houseVBO, houseIBO);
	initMesh(groundModel, groundModelFilename, false, groundVBO, groundIBO);
	
	// TODO Temp
	const ModelOBJ::Vertex *vb = groundModel.getVertexBuffer();

	for (int i = 0; i < groundModel.getNumberOfVertices(); i++) {
	cout << "vb[" << i << "].position: (" << vb[i].position[0] << "," << vb[i].position[1] << "," << vb[i].position[2] << ")" << endl;
	}

	for (int i = 0; i < groundModel.getNumberOfVertices(); i++) {
	cout << "vb[" << i << "].normal: (" << vb[i].normal[0] << "," << vb[i].normal[1] << "," << vb[i].normal[2] << ")" << endl;
	}

	for (int i = 0; i < groundModel.getNumberOfVertices(); i++) {
	cout << "vb[" << i << "].texCoord: (" << vb[i].texCoord[0] << "," << vb[i].texCoord[1] << ")" << endl;
	}
	

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

// ************************************************************************************************
// *** OpenGL callbacks implementation ************************************************************
/// Called whenever the scene has to be drawn
void display() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	// Set translation and scaling
    //GLint trULocation = glGetUniformLocation(ShaderProgram, "translation");
    //GLint sULocation = glGetUniformLocation(ShaderProgram, "scaling");
    //assert(trULocation != -1 && sULocation != -1);  
    //glUniform3fv(trULocation, 1, Translation.get());
    //glUniform1f(sULocation, Scaling);

	Cam.ar = (1.0f * width) / height;
	Matrix4f camTransformation = computeCameraTransform(Cam);

	glUniformMatrix4fv(TrLocation, 1, GL_FALSE, camTransformation.get());
   
	// Set the uniform variable for the texture unit (texture unit 0)
	//glUniform1i(SamplerLocation, 0);

	glUniform3fv(CameraPositionLoc, 1, Cam.position.get());

	// Set the light parameters
	glUniform3f(DLightDirLoc, 0.5f, -1.5f, -1.0f); // used
	glUniform3f(DLightAColorLoc, 0.5f, 0.3f, 0.0f); // used
	glUniform3f(DLightDColorLoc, 0.5f, 0.4f, 0.3f);
	glUniform3f(DLightSColorLoc, 0.6f, 0.6f, 0.7f);
	glUniform1f(DLightAIntensityLoc, 1.0f); // used
	glUniform1f(DLightDIntensityLoc, 1.0f); // used
	glUniform1f(DLightSIntensityLoc, 1.0f); // currently unused

	// Set the light parameters
	glUniform3f(PLightDirLoc, Cam.position.x(), Cam.position.y(), Cam.position.z()); // TODO currently not used, should be called position
	glUniform3f(PLightAColorLoc, 0.5f, 0.3f, 0.0f); //
	glUniform3f(PLightDColorLoc, 0.5f, 0.4f, 0.3f);
	glUniform3f(PLightSColorLoc, 0.6f, 0.6f, 0.7f);
	glUniform1f(PLightAIntensityLoc, 1.0f); // 
	glUniform1f(PLightDIntensityLoc, 1.0f); // 
	glUniform1f(PLightDIntensityKConstLoc, 0.0f); // used
	glUniform1f(PLightDIntensityKLinearLoc, 0.0f); // used
	glUniform1f(PLightDIntensityKSquareLoc, 0.5f); // used
	glUniform1f(PLightSIntensityLoc, 1.0f); // 

	

	// START Draw Ground -> move this to own function later
	
	// Set the material parameters for the ground
	glUniform3f(MaterialAColorLoc, 0.2f, 0.2f, 0.2f); // used
	glUniform3f(MaterialDColorLoc, 0.5f, 0.5f, 0.5f); // used
	glUniform3f(MaterialSColorLoc, 0.3f, 0.3f, 0.3f); // used
	glUniform1f(MaterialShineLoc, 10.0f); // used

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundIBO);

	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex), reinterpret_cast<const GLvoid*>(20)); //TODO

	// Draw the elements on the GPU
	glDrawElements(GL_TRIANGLES, groundModel.getNumberOfIndices(), GL_UNSIGNED_INT, 0);
	
	// END Draw Ground

	

	// START Draw House -> move this to own function later

	// Set the material parameters for the house
	glUniform3f(MaterialAColorLoc, 0.5f, 0.5f, 0.5f); // used
	glUniform3f(MaterialDColorLoc, 1.0f, 0.8f, 0.8f); // used
	glUniform3f(MaterialSColorLoc, 0.5f, 0.5f, 0.5f); // used
	glUniform1f(MaterialShineLoc, 20.0f); // used

										  // Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, houseIBO);

	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex), reinterpret_cast<const GLvoid*>(20)); //TODO

																		// Draw the elements on the GPU
	glDrawElements(GL_TRIANGLES, houseModel.getNumberOfIndices(), GL_UNSIGNED_INT, 0);

	// END Draw House
	

	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();	
}

/// Called at regular intervals (can be used for animations)
void idle() {
}

/// Called whenever a keyboard button is pressed (only ASCII characters)
void keyboard(unsigned char key, int x, int y) {
	Vector3f right;
	switch(tolower(key)) {
	case 'r': // Reset camera status
		initCamera(Cam);
		break;
	case 'w':
		Cam.position += Cam.target * 0.1f;
		break;
	case 'a':
		right = Cam.target.cross(Cam.up);
		Cam.position -= right * 0.1f;
		break;
	case 's':
		Cam.position -= Cam.target * 0.1f;
		break;
	case 'd':
		right = Cam.target.cross(Cam.up);
		Cam.position += right * 0.1f;
		break;
	case 'c':
		Cam.position -= Cam.up * 0.1f;
		break;
	case ' ':
		Cam.position += Cam.up * 0.1f;
		break;
	case 'n':	// Increase field of view
		Cam.fov = min(Cam.fov + 1.f, 179.f);
		break;
	case 'm':	// Decrease field of view
		Cam.fov = max(Cam.fov - 1.f, 1.f);
		break;
	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'q':  // terminate the application
		exit(0);
		break;
	case 'b':  // switch perspectiv to orth
		if (prjType == 0) {
			prjType = 1;
		} else {
			prjType = 0;
		}
		display();
		break;
	case 'i':  // print info about camera
		cout << "Camera Info:" << endl;
		cout << "Position: (" << Cam.position.x() << "," << Cam.position.y() << "," << Cam.position.z() << ")" << endl;
		cout << "Target: (" << Cam.target.x() << "," << Cam.target.y() << "," << Cam.target.z() << ")" << endl;
		cout << "Up: (" << Cam.up.x() << "," << Cam.up.y() << "," << Cam.up.z() << ")" << endl;
		cout << "Field of view: " << Cam.fov << endl;
		cout << "Aspect Ratio: " << Cam.ar << endl;
		cout << "Depth of the near plane: " << Cam.zNear << endl;
		cout << "Depth of the far plane: " << Cam.zFar << endl;
		cout << "Zoom: " << Cam.zoom << endl;
		break;
	case 'p': // change to wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'o': // change to polygon rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'l':
		cout << "Re-loading shaders..." << endl;
		if(initShaders()) {
			cout << "> done." << endl;
		}		
	}
	glutPostRedisplay();
}

/// Called whenever a mouse event occur (press or release)
void mouse(int button, int state, int x, int y) {
	// Store the current mouse status
	MouseButton = button;
	MouseX = x;	
	MouseY = y;
}

/// Called whenever the mouse is moving while a button is pressed
void motion(int x, int y) {
	if (MouseButton == GLUT_RIGHT_BUTTON) {
		Cam.position += Cam.target.cross(Cam.up) * 0.003f * (x - MouseX);
		Cam.position += Cam.target * 0.003f * (MouseY - y);

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		Cam.zoom = max(0.001f, Cam.zoom + 0.003f * (y - MouseY));		
		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		Matrix4f ry2, rr;

		// "horizontal" rotation
		ry2.rotate(0.1f * (MouseX - x), Vector3f(0, 1, 0));
		Cam.target = ry2 * Cam.target;
		Cam.up = ry2 * Cam.up;

		// "vertical" rotation
		rr.rotate(0.1f * (MouseY - y), Cam.target.cross(Cam.up));
		Cam.up = rr * Cam.up;
		Cam.target = rr * Cam.target;

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}

	glutPostRedisplay(); // Specify that the scene needs to be updated
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
bool initMesh(ModelOBJ& Model, string filename, bool normalize, GLuint& VBO, GLuint& IBO) {
	// Load the OBJ model
	if(!Model.import(filename.c_str())) {  // "capsule\\FinalBuilding.obj" //FinalBuilding.obj // capsule.obj // cube.obj // blendercube
		cerr << "Error: cannot load model." << endl;
		return false;
	}
	
	if (normalize) {
		Model.normalize();
	}

	if (Model.hasPositions()) {
		cout << "Model has positions" << endl;
	} else {
		cout << "Model does not have positions" << endl;
	}
	if (Model.hasTextureCoords()) {
		cout << "Model has texture coords" << endl;
	} else {
		cout << "Model does not have texture coords" << endl;
	}
	if (Model.hasNormals()) {
		cout << "Model has normals" << endl;
	} else {
		cout << "Model does not have normals" << endl;
	}

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
		Model.getNumberOfVertices() * sizeof(ModelOBJ::Vertex), 
		Model.getVertexBuffer(),
		GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * Model.getNumberOfTriangles() * sizeof(int),
		Model.getIndexBuffer(),
		GL_STATIC_DRAW);
	
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


	


    	
	
	return true;
} /* initBuffers() */


/// Initialize shaders. Return false if initialization fail
bool initShaders() {
	// Create the shader program and check for errors
	if(ShaderProgram != 0)
		glDeleteProgram(ShaderProgram);
	ShaderProgram = glCreateProgram();
	if(ShaderProgram == 0) {
		cerr << "Error: cannot create shader program." << endl;
		return false;
	}

	// Create the shader objects and check for errors
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if(vertShader == 0 || fragShader == 0) {
		cerr << "Error: cannot create shader objects." << endl;
		return false;
	}

	// Read and set the source code for the vertex shader
	string text = readTextFile("shader.v.glsl");
	const char* code = text.c_str();
	int length = static_cast<int>(text.length());
	if(length == 0)
		return false;
	glShaderSource(vertShader, 1, &code, &length);

	// Read and set the source code for the fragment shader
	string text2 = readTextFile("shader.f.glsl");
	const char *code2 = text2.c_str();
	length = static_cast<int>(text2.length());
	if(length == 0)
		return false;
	glShaderSource(fragShader, 1, &code2, &length);

	// Compile the shaders
	glCompileShader(vertShader);
	glCompileShader(fragShader);

	// Check for compilation error
	GLint success;
	GLchar errorLog[1024];
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile vertex shader.\nError log:\n" << errorLog << endl;
		return false;
	}
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile fragment shader.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Attach the shader to the program and link it
	glAttachShader(ShaderProgram, vertShader);
	glAttachShader(ShaderProgram, fragShader);
	glLinkProgram(ShaderProgram);

	// Check for linking error
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(ShaderProgram);

	// Check for validation error
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if(!success) {
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
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
} /* initShaders() */


// TODO Move to own Camera class
/// Return the transformation matrix corresponding to the specified camera
Matrix4f computeCameraTransform(const Camera& cam) {
	// camera rotation
	Vector3f t = cam.target.getNormalized();
	Vector3f u = cam.up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,
		u.x(), u.y(), u.z(), 0.f,
		-t.x(), -t.y(), -t.z(), 0.f,
		0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-cam.position);
	Matrix4f prj;

	if (prjType == 1) {
		// orthographic projection
		prj = Matrix4f::createOrthoPrj(-0.25f*cam.ar, 0.25f*cam.ar, -0.25f*cam.ar, 0.25f*cam.ar, cam.zNear, cam.zFar);
	}
	else {
		// perspective projection
		prj = Matrix4f::createPerspectivePrj(cam.fov, cam.ar, cam.zNear, cam.zFar);
	}


	

	// scaling due to zooming
	Matrix4f camZoom = Matrix4f::createScaling(cam.zoom, cam.zoom, 1.f);

	// Final transformation. Notice the multiplication order
	// First vertices are moved in camera space
	// Then the perspective projection puts them in clip space
	// And a final zooming factor is applied in clip space
	return camZoom * prj * camR  * camT;

} /* computeCameraTransform() */


// TODO Move to own Camera class
void initCamera(Camera& cam) {
	cam.position.set(-1.3f, 0.5f, 0.6f);
	cam.target.set(0.8f, -0.4f, -0.4f);
	cam.up.set(0.4f, 1.f, -0.2f);
	cam.fov = 30.f;
	cam.ar = 1.f; // will be correctly initialized in the "display()" method
	cam.zNear = 0.1f;
	cam.zFar = 100.f;
	cam.zoom = 1.f;
} /* initCamera() */

/// Read the specified file and return its content
string readTextFile(const string& pathAndFileName) {
    // Try to open the file
    ifstream fileIn(pathAndFileName);
    if(!fileIn.is_open()) {
        cerr << "Error: cannot open file "  << pathAndFileName.c_str();
        return "";
    }

    // Read the file
    string text = "";
    string line;
    while(!fileIn.eof()) {
        getline(fileIn, line);
        text += line + "\n";
        bool bad = fileIn.bad();
        bool fail = fileIn.fail();
        if(fileIn.bad() || (fileIn.fail() && !fileIn.eof())) {
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
} /* readTextFile() */

/* --- eof main.cpp --- */