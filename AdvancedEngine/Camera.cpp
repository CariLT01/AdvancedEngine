#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float NEAR = 0.1f;
const float FAR = 100000.f;


Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const float fov) {
	this->position = position;
	this->direction = direction;
	this->fov = fov;

	recomputeMatrices();
}

void Camera::recomputeMatrices() {
	this->direction = glm::normalize(direction);

	cameraRight = glm::cross(direction, worldUp);
	cameraUp = glm::cross(direction, cameraRight);

	viewMatrix = glm::lookAt(position, position + direction, cameraUp);
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, NEAR, FAR);
}