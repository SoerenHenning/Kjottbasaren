#include "Camera.h"
#include <algorithm>
#include <cmath>

Camera::Camera(Vector3f position, Vector3f target) {
	this->position = position;
	this->target = target;
	this->up = target.cross(Vector3f(0.0, 1.0, 0.0)).cross(target);

	this->fov = 30.f;
	this->ar = 1.f; // has to be correctly initialized in the "display()" method //TODO
	this->zNear = 0.1f;
	this->zFar = 100.f;
	this->zoom = 1.f;
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
	return this->fov;
}

void Camera::increaseFieldOfView(float value) {
	this->fov = min(max(this->fov + value, 1.f), 179.f);
}

float Camera::getAspectRatio() {
	return this->ar;
}

void Camera::setAspectRatio(float aspectRatio) {
	this->ar = aspectRatio;
}

float Camera::getNearPlane() {
	return this->zNear;
}

float Camera::getFarPlane() {
	return this->zFar;
}

float Camera::getZoom() {
	return this->zoom;
}

void Camera::zoomIn(float value) {
	this->zoom = max(0.001f, this->zoom + value);
}

Matrix4f Camera::getTransformationMatrix(Camera::Projection projection) {
	// camera rotation
	Vector3f t = this->target.getNormalized();
	Vector3f u = this->up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,
		u.x(), u.y(), u.z(), 0.f,
		-t.x(), -t.y(), -t.z(), 0.f,
		0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-this->position);
	Matrix4f prj;

	switch (projection) {
	case Projection::ORTHOGRAPHIC:
		prj = Matrix4f::createOrthoPrj(-0.25f*this->ar, 0.25f*this->ar, -0.25f*this->ar, 0.25f*this->ar, this->zNear, this->zFar);
		break;
	default:
		prj = Matrix4f::createPerspectivePrj(this->fov, this->ar, this->zNear, this->zFar);
		break;
	}

	// scaling due to zooming
	Matrix4f camZoom = Matrix4f::createScaling(this->zoom, this->zoom, 1.f);

	// Final transformation. Notice the multiplication order
	// First vertices are moved in camera space
	// Then the perspective projection puts them in clip space
	// And a final zooming factor is applied in clip space
	return camZoom * prj * camR  * camT;
}

void Camera::printStatus() {
	cout << "Camera Info:" << endl;
	cout << "Position: (" << this->position.x() << "," << this->position.y() << "," << this->position.z() << ")" << endl;
	cout << "Target: (" << this->target.x() << "," << this->target.y() << "," << this->target.z() << ")" << endl;
	cout << "Up: (" << this->up.x() << "," << this->up.y() << "," << this->up.z() << ")" << endl;
	cout << "Field of view: " << this->fov << endl;
	cout << "Aspect Ratio: " << this->ar << endl;
	cout << "Depth of the near plane: " << this->zNear << endl;
	cout << "Depth of the far plane: " << this->zFar << endl;
	cout << "Zoom: " << this->zoom << endl;
}
