#include "Chunk.h"
#include "Material.h"
#include "Settings.h"

Chunk::Chunk(const glm::vec3& chunkPosition, const std::vector<float>& densities, const std::vector<unsigned int>& materials) : chunkPosition(chunkPosition), densities(densities), materials(materials) {

}

Chunk::~Chunk() {
	delete chunkObject;
	delete chunkMesh;
}

void Chunk::buildChunk(Material* material, MarchingCubeGenerator* generator, Camera* camera) {
	
	std::vector<float> vertices = generator->generateMesh(densities, materials, 1);
	std::vector<unsigned int> indices(vertices.size());

	for (unsigned int i = 0; i < vertices.size() / 6; i++) {
		indices[i] = i;
	}

	chunkMesh = new Mesh(vertices, indices, material);
	chunkObject = new WorldObject(chunkPosition * static_cast<float>(CHUNK_SIZE), glm::vec3(0), glm::vec3(1), chunkMesh, camera); 
	// The -1 is temporary to avoid seams between chunks


}

void Chunk::render() {
	chunkObject->render();
}

