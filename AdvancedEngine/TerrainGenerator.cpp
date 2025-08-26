#include "TerrainGenerator.h"
#include "Settings.h"
#include <iostream>

TerrainGenerator::TerrainGenerator() {

	fnSimplex = FastNoise::New<FastNoise::Simplex>();

}

GeneratedTerrainResult TerrainGenerator::generateTerrain(const unsigned int& chunkPosX, const unsigned int& chunkPosY, const unsigned int& chunkPosZ) {
	GeneratedTerrainResult result;

	std::vector<float> densities((CHUNK_SIZE + 1)* (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));
	//densities.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	std::vector<unsigned int> materials((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));

	std::vector<float> heightmap((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));

	const float scale = 0.03f;
	
	fnSimplex->GenUniformGrid2D(heightmap.data(), chunkPosX * CHUNK_SIZE, chunkPosZ * CHUNK_SIZE, CHUNK_SIZE + 1, CHUNK_SIZE + 1, scale, 69420);

	for (unsigned int y = 0; y < CHUNK_SIZE + 1; y++) {
		for (unsigned int x = 0; x < CHUNK_SIZE + 1; x++) {
			for (unsigned int z = 0; z < CHUNK_SIZE + 1; z++) {
				const float heightMapValue = (heightmap[z * (CHUNK_SIZE + 1) + x] + 1) / 2.0f;
				//const float heightMapValue = fnSimplex->GenSingle2D((float)(x + chunkPosX * (CHUNK_SIZE + 1)) * scale, (float)(z + chunkPosZ * (CHUNK_SIZE + 1)) * scale, 69420);
				const float surfaceHeight = heightMapValue * 10.0f;

				const int worldX = x + chunkPosX * CHUNK_SIZE;
				const int worldY = y + chunkPosY * CHUNK_SIZE;
				const int worldZ = z + chunkPosZ * CHUNK_SIZE;

				const unsigned index = z + x * (CHUNK_SIZE + 1) + y * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1);

				const float density = std::clamp(surfaceHeight - worldY, 0.0f, 1.0f);

				if (index > (CHUNK_SIZE + 1)* (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1)) {
					std::cerr << "Index out of bounds: " << index << std::endl;
				}


				densities[index] = density;
			}
		}
	}

	return {
		.densities = densities,
		.materials = materials
	};
}

