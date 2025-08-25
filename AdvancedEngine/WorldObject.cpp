#include "WorldObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

WorldObject::WorldObject(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, Mesh* mesh, Camera* camera) {
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->mesh = mesh;
	this->camera = camera;
	recomputeModelMatrix();
}

void WorldObject::recomputeModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, scale);
}

void WorldObject::render() {
	mesh->prepareUniforms();

	int modelMatrixLoc = mesh->material->getUniformLocation("uModelMatrix");
	int viewMatrixLoc = mesh->material->getUniformLocation("uViewMatrix");
	int projectionMatrixLoc = mesh->material->getUniformLocation("uProjectionMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(camera->projectionMatrix));	

	mesh->render();
}