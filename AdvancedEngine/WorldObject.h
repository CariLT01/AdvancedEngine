#pragma once


#include "Mesh.h"
#include "Camera.h"

class WorldObject {
public:
	WorldObject(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, Mesh* mesh, Camera* camera);
	~WorldObject();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 modelMatrix;

	Mesh* mesh;
	Camera* camera;

	void recomputeModelMatrix();

	void render();
	void prepareMaterial();
};