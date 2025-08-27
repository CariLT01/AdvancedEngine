#include "ChunksManager.h"
#include <iostream>
#include <algorithm>

const unsigned int RENDER_DISTANCE = 6;

inline std::size_t hashVec3(const glm::vec3& v) {
	return std::hash<int>()(static_cast<int>(v.x))
		^ (std::hash<int>()(static_cast<int>(v.y)) << 1)
		^ (std::hash<int>()(static_cast<int>(v.z)) << 2);
}

ChunksManager::ChunksManager(Camera* camera, PhysicsEngine* physicsEngine) : camera(camera), physicsEngine(physicsEngine) {
	createOffsetsCache();

	terrainGenerator = new TerrainGenerator();
	terrainMaterial = new TerrainGBufferMaterial();

	// Initialize textures

	Texture* albedoTerrainTexture = new Texture("assets/terrain.png");
	Texture* normalTerrainTexture = new Texture("assets/terrain-normal.png");

	terrainMaterial->albedoTexture = albedoTerrainTexture;
	terrainMaterial->normalMapTexture = normalTerrainTexture;

	meshGenerator = new MarchingCubeGenerator(0.5f);
}

TerrainChunkData ChunksManager::generateChunk(const glm::vec3& chunkPosition) {
	GeneratedTerrainResult result = terrainGenerator->generateTerrain(chunkPosition.x, chunkPosition.y, chunkPosition.z);

	return {
		.x = static_cast<int>(chunkPosition.x),
		.y = static_cast<int>(chunkPosition.y),
		.z = static_cast<int>(chunkPosition.z),
		.densities = result.densities,
		.materials = result.materials
	};
}

void ChunksManager::tick(const glm::vec3& currentChunkPosition) {
	
	const std::vector<glm::vec3> loadList = createLoadList(currentChunkPosition, true); // Returns multiple values, add the ability to load multiple chunks per frame in the future (multithreading?)

	if (loadList.empty()) return;

	std::cout << "# Chunks left to load: " << loadList.size() << std::endl;

	// Load one chunk. Only one. FOr now...

	const glm::vec3& chunkToLoad = loadList[0];
	const size_t hash = hashVec3(chunkToLoad);

	if (knownChunks.contains(hash) == false) {
		// Generate chunk
		knownChunks[hash] = generateChunk(chunkToLoad);
	}

	// Load the chunk

	// TODO: load it with marching cubes generator


	const TerrainChunkData& chunkData = knownChunks[hash];

	Chunk* newChunk = new Chunk(chunkToLoad, chunkData.densities, chunkData.materials);
	newChunk->buildChunk(terrainMaterial, meshGenerator, camera, physicsEngine);
	if (newChunk->chunkBody != nullptr) {
		physicsEngine->addObject(newChunk->chunkBody);
	}


	loadedChunks[hash] = newChunk;

	// Unload any

	const std::vector<glm::vec3> fullLoadList = createLoadList(currentChunkPosition, false);

	std::vector<size_t> hashesToUnload;

	for (const auto& [hash, chunk] : loadedChunks) {

		if (chunk == nullptr) continue;

		if (std::find(fullLoadList.begin(), fullLoadList.end(), chunk->chunkPosition) == fullLoadList.end()) {
			// Unload

			delete chunk;
			loadedChunks[hash] = nullptr;
			hashesToUnload.push_back(hash);

			std::cout << "Delete chunk! " << std::endl;
		}
	}

	for (const size_t h : hashesToUnload) {
		loadedChunks.erase(h);
	}
}

void ChunksManager::createOffsetsCache() {
	for (int x = -(int)RENDER_DISTANCE; x <= (int)RENDER_DISTANCE; x++) {
		for (int y = -(int)RENDER_DISTANCE; y <= (int)RENDER_DISTANCE; y++) {
			for (int z = -(int)RENDER_DISTANCE; z <= (int)RENDER_DISTANCE; z++) {
				if (x * x + y * y + z * z <= RENDER_DISTANCE * RENDER_DISTANCE) {
					loadChunksOffsets.emplace_back(x, y, z);
				}
			}
		}
	}

	std::cout << "Created load offsets cache with " << loadChunksOffsets.size() << " entries." << std::endl;
}

void ChunksManager::renderChunks() {
	for (const auto& [hash, chunk] : loadedChunks) {
		chunk->render();
	}
}

std::vector<glm::vec3> ChunksManager::createLoadList(const glm::vec3& currentChunkPosition, const bool ignoreCurrentlyLoaded) {
	std::vector<glm::vec3> toLoad;
	toLoad.reserve(loadChunksOffsets.size());

	unsigned int index = 0;
	for (const glm::vec3& offset : loadChunksOffsets) {
		const glm::vec3 chunkPosition = currentChunkPosition + offset;
		const size_t hash = hashVec3(chunkPosition);

		if (loadedChunks.contains(hash) && ignoreCurrentlyLoaded) continue;
		toLoad.emplace_back(chunkPosition);
	}
	return toLoad;
}

