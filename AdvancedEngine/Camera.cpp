#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float NEAR = 0.1f;
const float FAR = 100000.f;


Camera::Camera(const glm::vec3& position, const float fov) : pitch(0.0f), yaw(0.0f), roll(0.0f) {
	this->position = position;
	this->fov = fov;

	recomputeMatrices();
}

void Camera::recomputeMatrices() {
    // Convert yaw/pitch/roll to radians
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);
    float rollRad = glm::radians(roll);

    // Compute the front (direction) vector
    glm::vec3 front;
    front.x = cos(pitchRad) * cos(yawRad);
    front.y = sin(pitchRad);
    front.z = cos(pitchRad) * sin(yawRad);
    direction = glm::normalize(front);

    // Compute the right vector
    cameraRight = glm::normalize(glm::cross(direction, worldUp));

    // Compute the up vector considering roll
    if (roll != 0.0f) {
        // Create a rotation matrix around the direction vector
        glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), rollRad, direction);
        // Transform the original worldUp vector to get the rolled cameraUp
        cameraUp = glm::normalize(glm::vec3(rollMat * glm::vec4(cameraRight, 0.0f)));
        // Recompute right to ensure orthogonality
        cameraRight = glm::normalize(glm::cross(cameraUp, direction));
    }
    else {
        cameraUp = glm::normalize(glm::cross(cameraRight, direction));
    }

    // Compute view matrix
    viewMatrix = glm::lookAt(position, position + direction, cameraUp);

    // Compute projection matrix
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, NEAR, FAR);
}

