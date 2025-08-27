#include "Player.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <iostream>

const float MOVEMENT_SPEED = 5.0f;

Player::Player(const glm::vec3& startingPosition, Camera* camera, PhysicsEngine* physicsEngine) : camera(camera), physicsEngine(physicsEngine) {
	const float radius = 0.5f;
	const float height = 1.8f;


	JPH::RefConst<JPH::Shape> capsuleShape = new JPH::CapsuleShape(height / 2, radius);

	JPH::BodyCreationSettings bcs(
		capsuleShape,
		JPH::Vec3(startingPosition.x, startingPosition.y, startingPosition.z),
		JPH::Quat::sIdentity(),
		JPH::EMotionType::Dynamic,
		Layers::MOVING
	);
	bcs.mAllowSleeping = false;

	playerBody = physicsEngine->bodyInterface->CreateBody(bcs);
	physicsEngine->addObject(playerBody);
}

void Player::processInputs(Window* window, const float deltaTime) {

	//std::cout << "process inputs begin read vel" << std::endl;
	JPH::Vec3 currentVelocity = physicsEngine->bodyReadVelocity(*playerBody);
	//std::cout << "end read vel begin read inputs" << std::endl;
	JPH::Vec3 newVelocity = currentVelocity;

	glm::vec3 movementVelocity = glm::vec3(0);

	// Calculate front

	glm::vec3 front;

	float yawRad = glm::radians(camera->yaw);
	float pitchRad = glm::radians(0.f);
	float rollRad = glm::radians(0.f);

	// Compute the front (direction) vector
	front.x = cos(pitchRad) * cos(yawRad);
	front.y = sin(pitchRad);
	front.z = cos(pitchRad) * sin(yawRad);

	front = glm::normalize(front);

	// Calculate right
	const glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));

	

	if (window->getKeyPressed(GLFW_KEY_W) == GLFW_PRESS) {
		movementVelocity += front * MOVEMENT_SPEED * deltaTime; // Temporary
	}
	if (window->getKeyPressed(GLFW_KEY_A) == GLFW_PRESS) {
		movementVelocity += -right * MOVEMENT_SPEED * deltaTime;
	}
	if (window->getKeyPressed(GLFW_KEY_S) == GLFW_PRESS) {
		movementVelocity += -front * MOVEMENT_SPEED * deltaTime;
	}
	if (window->getKeyPressed(GLFW_KEY_D) == GLFW_PRESS) {
		movementVelocity += right * MOVEMENT_SPEED * deltaTime;
	}

	newVelocity += JPH::Vec3(
		movementVelocity.x,
		movementVelocity.y,
		movementVelocity.z
	);

	//std::cout << "end read inputs begin write vel" << std::endl;
	physicsEngine->bodyWriteVelocity(newVelocity, *playerBody);

	JPH::Vec3 zero = JPH::Vec3::sZero();

	physicsEngine->bodyWriteAngularVelocity(zero, *playerBody);

}

void Player::postUpdate() {
	const JPH::Vec3& bodyPosition = physicsEngine->getBodyLocation(*playerBody);

	camera->position = glm::vec3(bodyPosition.GetX(), bodyPosition.GetY(), bodyPosition.GetZ());
	camera->recomputeMatrices();
}