#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "PhysicsEngine.h"
#include "Window.h"

class Player {
public:
	Player(const glm::vec3& startingPosition, Camera* camera, PhysicsEngine* physicsEngine);

	void processInputs(Window* window, const float deltaTime);
	void createBody();
	void postUpdate();

	Camera* camera;
	JPH::Body* playerBody;
	PhysicsEngine* physicsEngine;
};