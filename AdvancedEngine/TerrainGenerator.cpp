#include "TerrainGenerator.h"
#include "Settings.h"
#include <iostream>

TerrainGenerator::TerrainGenerator() {

	fnSimplex = FastNoise::New<FastNoise::Simplex>();
	fnFractal = FastNoise::New<FastNoise::FractalFBm>();
	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(6);

	fnCaveFractal = FastNoise::New<FastNoise::FractalFBm>();
	fnCaveFractal->SetSource(fnSimplex);
	fnCaveFractal->SetOctaveCount(5);
}

GeneratedTerrainResult TerrainGenerator::generateTerrain(const unsigned int& chunkPosX, const unsigned int& chunkPosY, const unsigned int& chunkPosZ) {
	GeneratedTerrainResult result;

	std::vector<float> densities((CHUNK_SIZE + 1)* (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));
	//densities.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	std::vector<unsigned int> materials((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));

	std::vector<float> heightmap((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));
	std::vector<float> caveMap((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1));

	const float scale = 0.002f;
	const float caveScale = 0.005f;
	const float transition = 1.0f;

	fnFractal->GenUniformGrid2D(heightmap.data(), chunkPosX * CHUNK_SIZE, chunkPosZ * CHUNK_SIZE, CHUNK_SIZE + 1, CHUNK_SIZE + 1, scale, 69420);
	fnCaveFractal->GenUniformGrid3D(caveMap.data(), chunkPosX * CHUNK_SIZE, chunkPosY * CHUNK_SIZE, chunkPosZ * CHUNK_SIZE, CHUNK_SIZE + 1, CHUNK_SIZE + 1, CHUNK_SIZE + 1, caveScale, 69420);

	for (unsigned int y = 0; y < CHUNK_SIZE + 1; y++) {
		for (unsigned int x = 0; x < CHUNK_SIZE + 1; x++) {
			for (unsigned int z = 0; z < CHUNK_SIZE + 1; z++) {




				const int worldX = x + chunkPosX * CHUNK_SIZE;
				const int worldY = y + chunkPosY * CHUNK_SIZE;
				const int worldZ = z + chunkPosZ * CHUNK_SIZE;

				const unsigned index = z + x * (CHUNK_SIZE + 1) + y * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1);
				const unsigned indexCave = x + y * (CHUNK_SIZE + 1) + z * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1);

				float caveDensity = (0.95f - caveMap[indexCave]) / transition;
				caveDensity = std::clamp(caveDensity, 0.0f, 1.0f);

				const float heightMapValue = (heightmap[z * (CHUNK_SIZE + 1) + x] + 1) / 2.0f;

				const float realHeight = powf(heightMapValue, 3);

				//const float heightMapValue = fnSimplex->GenSingle2D((float)(x + chunkPosX * (CHUNK_SIZE + 1)) * scale, (float)(z + chunkPosZ * (CHUNK_SIZE + 1)) * scale, 69420);
				const float surfaceHeight = realHeight * 250.0f;

				float density = (surfaceHeight - worldY) / transition;
				// Now density crosses 0 at surfaceHeight
				density = std::clamp(density * 0.5f + 0.5f, 0.0f, 1.0f);

				if (index > (CHUNK_SIZE + 1)* (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1)) {
					std::cerr << "Index out of bounds: " << index << std::endl;
				}



				densities[index] = density * caveDensity;
				
				if (worldY < surfaceHeight - 2) {
					materials[index] = 0;
				}
				else {
					materials[index] = 2;
				}


				
				
			}
		}
	}

	return {
		.densities = densities,
		.materials = materials
	};
}

