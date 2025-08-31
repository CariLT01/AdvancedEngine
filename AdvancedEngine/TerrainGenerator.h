#pragma once

#include <FastNoise/FastNoise.h>
#include <vector>

struct GeneratedTerrainResult {
	std::vector<float> densities;
	std::vector<unsigned int> materials;
};

class TerrainGenerator {

public:
	TerrainGenerator();

	GeneratedTerrainResult generateTerrain(const unsigned int& chunkPosX, const unsigned int& chunkPosY, const unsigned int& chunkPosZ);

	FastNoise::SmartNode<FastNoise::FractalFBm> fnFractal;
	FastNoise::SmartNode<FastNoise::Simplex> fnSimplex;
	FastNoise::SmartNode<FastNoise::FractalFBm> fnCaveFractal;
};