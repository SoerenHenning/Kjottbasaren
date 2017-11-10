#pragma once

#include "Vector3.h"
#include "Matrix4.h"

using namespace std;

class Camera {

	public:
		enum class Projection { ORTHOGRAPHIC, PERSPECTIVE };

	Vector3f position;			// position of the camera
	Vector3f target;			// the direction the camera is looking at
	Vector3f up;				//  the up vector of the camera
	float fieldOfView;			// camera field of view
	float aspectRatio;			// camera aspect ratio
	float nearPlane, farPlane;	// depth of the near and far plane
	float zoom;					// an additional scaling parameter
	Camera::Projection projectionType;

	//TODO preparation for new assignment
	float alpha = 0.0; //TODO temp
	float distance = 2.0; //TODO temp

	public:
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
		Camera::Projection getProjection();
		void setProjection(Camera::Projection);
		void toggleProjection();
		Matrix4f getTransformationMatrix();
		void printStatus();

		//TODO preparation for new assignment
		void drive(float); //TODO temp here
};

