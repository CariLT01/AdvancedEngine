#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "WorldObject.h"
#include "MarchingCubesGenerator.h"
#include "PhysicsEngine.h"

class Chunk {
public:
	Chunk(const glm::vec3& chunkPosition, const std::vector<float>& densities, const std::vector<unsigned int>& materials);
	~Chunk();

	glm::vec3 chunkPosition;
	std::vector<float> densities;
	std::vector<unsigned int> materials;

	WorldObject* chunkObject;
	Mesh* chunkMesh;
	JPH::Body* chunkBody;
	JPH::Ref<JPH::Shape> chunkShape;
	PhysicsEngine* physicsEngine;

	void buildChunk(Material* material, MarchingCubeGenerator* generator, Camera* camera, PhysicsEngine* physicsEngine);
	void render();
};