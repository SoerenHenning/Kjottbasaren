#include "Vector3.h"
#include "Matrix4.h"

using namespace std;

class Camera {

	Vector3f position;			// position of the camera
	Vector3f target;			// the direction the camera is looking at
	Vector3f up;				//  the up vector of the camera
	float fieldOfView;			// camera field of view
	float aspectRatio;			// camera aspect ratio
	float nearPlane, farPlane;	// depth of the near and far plane
	float zoom;					// an additional scaling parameter

	public:
		enum class Projection { ORTHOGRAPHIC, PERSPECTIVE };

		Camera(Vector3f, Vector3f);
		~Camera();
		Vector3f getPosition();
		void moveForward(float);
		void moveRight(float);
		void moveUp(float);
		Vector3f getTarget();
		Vector3f getUp();
		void rotateHorizontal(float);
		void rotateVertical(float);
		float getFieldOfView();
		void increaseFieldOfView(float);
		float getAspectRatio();
		void setAspectRatio(float);
		float getNearPlane();
		float getFarPlane();
		float getZoom();
		void zoomIn(float);
		Matrix4f getTransformationMatrix(Camera::Projection);
		void printStatus();
};

