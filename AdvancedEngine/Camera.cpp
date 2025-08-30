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

    extractPlanes();
}


void Camera::extractPlanes() {

    glm::mat4 VP = projectionMatrix * viewMatrix;

    planes[0].normal.x = VP[0][3] + VP[0][0];
    planes[0].normal.y = VP[1][3] + VP[1][0];
    planes[0].normal.z = VP[2][3] + VP[2][0];
    planes[0].distance = VP[3][3] + VP[3][0];

    // Right
    planes[1].normal.x = VP[0][3] - VP[0][0];
    planes[1].normal.y = VP[1][3] - VP[1][0];
    planes[1].normal.z = VP[2][3] - VP[2][0];
    planes[1].distance = VP[3][3] - VP[3][0];

    // Bottom
    planes[2].normal.x = VP[0][3] + VP[0][1];
    planes[2].normal.y = VP[1][3] + VP[1][1];
    planes[2].normal.z = VP[2][3] + VP[2][1];
    planes[2].distance = VP[3][3] + VP[3][1];

    // Top
    planes[3].normal.x = VP[0][3] - VP[0][1];
    planes[3].normal.y = VP[1][3] - VP[1][1];
    planes[3].normal.z = VP[2][3] - VP[2][1];
    planes[3].distance = VP[3][3] - VP[3][1];

    // Near
    planes[4].normal.x = VP[0][3] + VP[0][2];
    planes[4].normal.y = VP[1][3] + VP[1][2];
    planes[4].normal.z = VP[2][3] + VP[2][2];
    planes[4].distance = VP[3][3] + VP[3][2];

    // Far
    planes[5].normal.x = VP[0][3] - VP[0][2];
    planes[5].normal.y = VP[1][3] - VP[1][2];
    planes[5].normal.z = VP[2][3] - VP[2][2];
    planes[5].distance = VP[3][3] - VP[3][2];

    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(planes[i].normal);
        planes[i].normal /= length;
        planes[i].distance /= length;
    }
}

bool Camera::isAABBoutsidePlane(const Plane& plane, const glm::vec3& min, const glm::vec3& max) {
    // Positive vertex
    glm::vec3 p = min;
    if (plane.normal.x >= 0) p.x = max.x;
    if (plane.normal.y >= 0) p.y = max.y;
    if (plane.normal.z >= 0) p.z = max.z;

    // If positive vertex is behind plane, box is outside
    return glm::dot(plane.normal, p) + plane.distance < 0;
}

bool Camera::isAABBinsideFrustum(const glm::vec3& min, const glm::vec3& max) {
    for (int i = 0; i < 6; i++) {
        if (isAABBoutsidePlane(planes[i], min, max)) return false;
    }
    return true;
}

bool Camera::IsAABBboxInsideFrustum(const glm::vec3 & position, const glm::vec3 & size) {
    glm::vec3 halfSize = size * 0.5f;
    glm::vec3 min = position - halfSize;
    glm::vec3 max = position + halfSize;
    return isAABBinsideFrustum(min, max);
}