#pragma once

#include <vector>

struct MarchingCubesResult {
	std::vector<float> vertices; // x, y, z, nx, ny, nz
	std::vector<unsigned int> indices;
};

class MarchingCubeGenerator {
public:
	MarchingCubeGenerator(const float& threshold);

	std::vector<float> generateMesh(std::vector<float> densities, std::vector<unsigned int> materials, const unsigned int& detailLevel);

	float threshold;
private:
	std::vector<float> buildCell(const unsigned int& localX, const unsigned int& localY, const unsigned int& localZ, std::vector<float>& densities, std::vector<unsigned int>& materials, const unsigned int& detailLevel);

	float getDensityAtPoint(std::vector<float>& densities, const unsigned int& x, const unsigned int& y, const unsigned int& z);
};