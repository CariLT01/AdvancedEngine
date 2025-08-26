#pragma once

#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>
#include "TerrainGenerator.h"
#include "Chunk.h"
#include "MoreMaterials.h"

struct TerrainChunkData {
	int x;
	int y;
	int z;

	std::vector<float> densities;
	std::vector<unsigned int> materials;
};

class ChunksManager {
public:
	ChunksManager(Camera* camera);
	void tick(const glm::vec3& currentChunkPosition);
	void renderChunks();
private:

	std::vector<glm::vec3> createLoadList(const glm::vec3& currentChunkPosition, const bool ignoreCurrentlyLoaded);

	TerrainChunkData generateChunk(const glm::vec3& chunkPosition);


	void createOffsetsCache();

	std::unordered_map<size_t, TerrainChunkData> knownChunks;
	std::unordered_map<size_t, Chunk*> loadedChunks;

	std::vector<glm::vec3> loadChunksOffsets;
	
	TerrainGenerator* terrainGenerator;
	TerrainMaterial* terrainMaterial;
	MarchingCubeGenerator* meshGenerator;
	Camera* camera;
	


};