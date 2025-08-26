#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "WorldObject.h"
#include "MarchingCubesGenerator.h"

class Chunk {
public:
	Chunk(const glm::vec3& chunkPosition, const std::vector<float>& densities, const std::vector<unsigned int>& materials);
	~Chunk();

	glm::vec3 chunkPosition;
	std::vector<float> densities;
	std::vector<unsigned int> materials;

	WorldObject* chunkObject;
	Mesh* chunkMesh;


	void buildChunk(Material* material, MarchingCubeGenerator* generator, Camera* camera);
	void render();
};