#pragma once

#include <glm/glm.hpp>


struct Plane {
	glm::vec3 normal;
	float distance;
};

class Camera {
public:
	Camera(const glm::vec3& position, const float fov);
	~Camera();

	glm::vec3 position;
	glm::vec3 direction;


	glm::vec3 cameraUp;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;

	float fov;
	float aspectRatio;

	float pitch;
	float yaw;
	float roll;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	Plane planes[6];

	void recomputeMatrices();

	void update();
	void extractPlanes();
	bool isAABBoutsidePlane(const Plane& plane, const glm::vec3& min, const glm::vec3& max);
	bool isAABBinsideFrustum(const glm::vec3& min, const glm::vec3& max);
	bool IsAABBboxInsideFrustum(const glm::vec3& position, const glm::vec3& size);

};