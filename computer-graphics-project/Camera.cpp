#include "Camera.h"
#include <algorithm>
#include <cmath>

Camera::Camera(Vector3f position, Vector3f target) {
	this->position = position;
	this->target = target;
	this->up = target.cross(Vector3f(0.0, 1.0, 0.0)).cross(target);

	this->fieldOfView = 30.f;
	this->aspectRatio = 1.f;
	this->nearPlane = 0.1f;
	this->farPlane = 100.f;
	this->zoom = 1.f;

	this->projectionType = Camera::Projection::PERSPECTIVE;
}


Camera::~Camera() {
}

Vector3f Camera::getPosition() {
	return position;
}

void Camera::moveForward(float value) {
	this->position += this->target * value;
}

void Camera::moveRight(float value) {
	Vector3f right = this->target.cross(this->up);
	this->position += right * value;
}

void Camera::moveUp(float value) {
	this->position += this->up * value;
}

Vector3f Camera::getTarget() {
	return target;
}

Vector3f Camera::getUp() {
	return up;
}

void Camera::rotateHorizontal(float value) {
	Matrix4f rotationMatrix;
	rotationMatrix.rotate(value, Vector3f(0, 1, 0));
	this->target = rotationMatrix * this->target;
	this->up = rotationMatrix * this->up;
}

void Camera::rotateVertical(float value) {
	Matrix4f rotationMatrix;
	rotationMatrix.rotate(value, this->target.cross(this->up));
	this->up = rotationMatrix * this->up;
	this->target = rotationMatrix * this->target;
}

float Camera::getFieldOfView() {
	return this->fieldOfView;
}

void Camera::increaseFieldOfView(float value) {
	this->fieldOfView = min(max(this->fieldOfView + value, 1.f), 179.f);
}

float Camera::getAspectRatio() {
	return this->aspectRatio;
}

void Camera::setAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
}

float Camera::getNearPlane() {
	return this->nearPlane;
}

float Camera::getFarPlane() {
	return this->farPlane;
}

float Camera::getZoom() {
	return this->zoom;
}

void Camera::zoomIn(float value) {
	this->zoom = max(0.001f, this->zoom + value);
}

Camera::Projection Camera::getProjection() {
	return this->projectionType;
}

void Camera::setProjection(Camera::Projection projection) {
	this->projectionType = projection;
}

void Camera::toggleProjection() {
	if (projectionType == Camera::Projection::PERSPECTIVE) {
		projectionType = Camera::Projection::ORTHOGRAPHIC;
	} else {
		projectionType = Camera::Projection::PERSPECTIVE;
	}
}

Matrix4f Camera::getTransformationMatrix() {
	// Camera Rotation
	Vector3f target = this->target.getNormalized();
	Vector3f up = this->up.getNormalized();
	Vector3f right = target.cross(up);
	Matrix4f rotation(right.x(), right.y(), right.z(), 0.f,
						up.x(), up.y(), up.z(), 0.f,
						-target.x(), -target.y(), -target.z(), 0.f,
						0.f, 0.f, 0.f, 1.f);

	// Camera Translation
	Matrix4f translation = Matrix4f::createTranslation(-this->position);
	
	// Projection
	Matrix4f projection;
	if (projectionType == Projection::ORTHOGRAPHIC) {
		float height = 0.5f;
		float width = height * this->aspectRatio;
		projection = Matrix4f::createOrthoPrj(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, this->nearPlane, this->farPlane);
	} else {
		projection = Matrix4f::createPerspectivePrj(this->fieldOfView, this->aspectRatio, this->nearPlane, this->farPlane);
	}

	// Zooming
	Matrix4f scaling = Matrix4f::createScaling(this->zoom, this->zoom, 1.f);

	return scaling * projection * rotation  * translation;
}

void Camera::printStatus() {
	cout << "Camera Info:" << endl;
	cout << "Position: (" << this->position.x() << "," << this->position.y() << "," << this->position.z() << ")" << endl;
	cout << "Target: (" << this->target.x() << "," << this->target.y() << "," << this->target.z() << ")" << endl;
	cout << "Up: (" << this->up.x() << "," << this->up.y() << "," << this->up.z() << ")" << endl;
	cout << "Field of view: " << this->fieldOfView << endl;
	cout << "Aspect Ratio: " << this->aspectRatio << endl;
	cout << "Depth of the near plane: " << this->nearPlane << endl;
	cout << "Depth of the far plane: " << this->farPlane << endl;
	cout << "Zoom: " << this->zoom << endl;
}

void Camera::drive(float delta) {
	alpha = fmod((alpha + delta), 360.0);
	float x = cos(alpha + delta) * distance;
	float z = sin(alpha + delta) * distance;
	position = Vector3f(x, 0.5f, z);
}
