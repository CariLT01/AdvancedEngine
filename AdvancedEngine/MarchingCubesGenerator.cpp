#include "MarchingCubesGenerator.h"
#include "Settings.h"
#include "TriangulationTables.h"
#include <cmath>
#include <unordered_map>
#include <glm/glm.hpp>
#include <iostream>

struct Vector3 {
	float x;
	float y;
	float z;

	// Member function for addition
	Vector3 add(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	// Operator overload for +
	Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	Vector3 operator*(const float& scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	// Optional: operator += for in-place addition
	Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
};

MarchingCubeGenerator::MarchingCubeGenerator(const float& threshold) : threshold(threshold) {

}

std::vector<float> MarchingCubeGenerator::generateMesh(std::vector<float> densities, std::vector<unsigned int> materials, const unsigned int& detailLevel) {
	std::vector<float> vertices = {};
	
	size_t notEmptyCount = 0;

	for (unsigned int x = 0; x < (CHUNK_SIZE / detailLevel)-1; x++) {
		for (unsigned int y = 0; y < (CHUNK_SIZE / detailLevel)-1; y++) {
			for (unsigned int z = 0; z < (CHUNK_SIZE / detailLevel)-1; z++) {

				

				const std::vector<float> cellVertices = buildCell(x, y, z, densities, materials, detailLevel);

				for (const float vertex : cellVertices) {
					notEmptyCount++;
					vertices.push_back(vertex);
				}

			}
		}
	}
	std::cout << "Marching Cubes generated " << notEmptyCount / 6 << " triangles." << std::endl;

	return vertices;
}

float MarchingCubeGenerator::getDensityAtPoint(std::vector<float>& densities, const unsigned int& x, const unsigned int& y, const unsigned int& z) {
	return densities[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

float sampleDensityAt(const std::vector<float>& densities, float x, float y, float z) {
	const int maxIdx = CHUNK_SIZE - 1; // node indices: 0..CHUNK_SIZE-1
	// clamp coordinates to [0, maxIdx]
	if (x < 0.0f) x = 0.0f;
	if (y < 0.0f) y = 0.0f;
	if (z < 0.0f) z = 0.0f;
	if (x > (float)maxIdx) x = (float)maxIdx;
	if (y > (float)maxIdx) y = (float)maxIdx;
	if (z > (float)maxIdx) z = (float)maxIdx;

	int x0 = (int)floorf(x);
	int x1 = (x0 < maxIdx) ? x0 + 1 : x0;
	int y0 = (int)floorf(y);
	int y1 = (y0 < maxIdx) ? y0 + 1 : y0;
	int z0 = (int)floorf(z);
	int z1 = (z0 < maxIdx) ? z0 + 1 : z0;

	float xd = (x1 == x0) ? 0.0f : (x - x0);
	float yd = (y1 == y0) ? 0.0f : (y - y0);
	float zd = (z1 == z0) ? 0.0f : (z - z0);

	auto get = [&](int xi, int yi, int zi)->float {
		size_t idx = (size_t)xi + (size_t)yi * CHUNK_SIZE + (size_t)zi * CHUNK_SIZE * CHUNK_SIZE;
		return densities[idx];
		};

	float c000 = get(x0, y0, z0);
	float c100 = get(x1, y0, z0);
	float c010 = get(x0, y1, z0);
	float c110 = get(x1, y1, z0);
	float c001 = get(x0, y0, z1);
	float c101 = get(x1, y0, z1);
	float c011 = get(x0, y1, z1);
	float c111 = get(x1, y1, z1);

	float c00 = c000 * (1 - xd) + c100 * xd;
	float c10 = c010 * (1 - xd) + c110 * xd;
	float c01 = c001 * (1 - xd) + c101 * xd;
	float c11 = c011 * (1 - xd) + c111 * xd;

	float c0 = c00 * (1 - yd) + c10 * yd;
	float c1 = c01 * (1 - yd) + c11 * yd;

	float c = c0 * (1 - zd) + c1 * zd;
	return c;
}


Vector3 VertexInterp(float isolevel, const Vector3& p1, const Vector3& p2, float valp1, float valp2) {
	float mu;
	Vector3 p;

	const float epsilon = 1e-5f;

	if (std::fabs(isolevel - valp1) < epsilon)
		return p1;
	if (std::fabs(isolevel - valp2) < epsilon)
		return p2;
	if (std::fabs(valp1 - valp2) < epsilon)
		return p1;

	mu = (isolevel - valp1) / (valp2 - valp1);

	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return p;
}

int hashVector(const Vector3& v) {
	size_t hash = std::hash<int>()(v.x);
	hash ^= std::hash<int>()(v.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= std::hash<int>()(v.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

struct VertexMapItem {
	Vector3 position;
	unsigned int index;
};

void addVertex(const Vector3& a, const Vector3& b, const Vector3& c, std::vector<float>& vertices) {
	glm::vec3 A = glm::vec3(a.x, a.y, a.z);
	glm::vec3 B = glm::vec3(b.x, b.y, b.z);
	glm::vec3 C = glm::vec3(c.x, c.y, c.z);

	glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));

	vertices.push_back(a.x);
	vertices.push_back(a.y);
	vertices.push_back(a.z);
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);

	vertices.push_back(b.x);
	vertices.push_back(b.y);
	vertices.push_back(b.z);
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);

	vertices.push_back(c.x);
	vertices.push_back(c.y);
	vertices.push_back(c.z);
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);
}



std::vector<float> MarchingCubeGenerator::buildCell(const unsigned int& localX, const unsigned int& localY, const unsigned int& localZ, std::vector<float>& densities, std::vector<unsigned int>& materials, const unsigned int& detailLevel) {
	std::vector<float> vertices;
	
	const Vector3 point0 = { localX, localY, localZ };
	const Vector3 point1 = { localX + 1, localY, localZ };
	const Vector3 point2 = { localX + 1, localY, localZ + 1 };
	const Vector3 point3 = { localX, localY, localZ + 1 };
	const Vector3 point4 = { localX, localY + 1, localZ };
	const Vector3 point5 = { localX + 1, localY + 1, localZ };
	const Vector3 point6 = { localX + 1, localY + 1, localZ + 1 };
	const Vector3 point7 = { localX, localY + 1, localZ + 1 };

	Vector3 vertList[12] = { {0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};

	constexpr float ISOLEVEL = 0.5f;

	unsigned int cubeIndex = 0;

	if (getDensityAtPoint(densities, point0.x * detailLevel, point0.y * detailLevel, point0.z * detailLevel) < ISOLEVEL) cubeIndex |= 1;
	if (getDensityAtPoint(densities, point1.x * detailLevel, point1.y * detailLevel, point1.z * detailLevel) < ISOLEVEL) cubeIndex |= 2;
	if (getDensityAtPoint(densities, point2.x * detailLevel, point2.y * detailLevel, point2.z * detailLevel) < ISOLEVEL) cubeIndex |= 4;
	if (getDensityAtPoint(densities, point3.x * detailLevel, point3.y * detailLevel, point3.z * detailLevel) < ISOLEVEL) cubeIndex |= 8;
	if (getDensityAtPoint(densities, point4.x * detailLevel, point4.y * detailLevel, point4.z * detailLevel) < ISOLEVEL) cubeIndex |= 16;
	if (getDensityAtPoint(densities, point5.x * detailLevel, point5.y * detailLevel, point5.z * detailLevel) < ISOLEVEL) cubeIndex |= 32;
	if (getDensityAtPoint(densities, point6.x * detailLevel, point6.y * detailLevel, point6.z * detailLevel) < ISOLEVEL) cubeIndex |= 64;
	if (getDensityAtPoint(densities, point7.x * detailLevel, point7.y * detailLevel, point7.z * detailLevel) < ISOLEVEL) cubeIndex |= 128;

	if (edgeTable[cubeIndex] == 0) {
		return {};
	}

	if (edgeTable[cubeIndex] & 1) {
		vertList[0] = VertexInterp(ISOLEVEL, point0 * detailLevel, point1 * detailLevel, getDensityAtPoint(densities, point0.x * detailLevel, point0.y * detailLevel, point0.z * detailLevel), getDensityAtPoint(densities, point1.x * detailLevel, point1.y * detailLevel, point1.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 2) {
		vertList[1] = VertexInterp(ISOLEVEL, point1 * detailLevel, point2 * detailLevel, getDensityAtPoint(densities, point1.x * detailLevel, point1.y * detailLevel, point1.z * detailLevel), getDensityAtPoint(densities, point2.x * detailLevel, point2.y * detailLevel, point2.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 4) {
		vertList[2] = VertexInterp(ISOLEVEL, point2 * detailLevel, point3 * detailLevel, getDensityAtPoint(densities, point2.x * detailLevel, point2.y * detailLevel, point2.z * detailLevel), getDensityAtPoint(densities, point3.x * detailLevel, point3.y * detailLevel, point3.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 8) {
		vertList[3] = VertexInterp(ISOLEVEL, point3 * detailLevel, point0 * detailLevel, getDensityAtPoint(densities, point3.x * detailLevel, point3.y * detailLevel, point3.z * detailLevel), getDensityAtPoint(densities, point0.x * detailLevel, point0.y * detailLevel, point0.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 16) {
		vertList[4] = VertexInterp(ISOLEVEL, point4 * detailLevel, point5 * detailLevel, getDensityAtPoint(densities, point4.x * detailLevel, point4.y * detailLevel, point4.z * detailLevel), getDensityAtPoint(densities, point5.x * detailLevel, point5.y * detailLevel, point5.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 32) {
		vertList[5] = VertexInterp(ISOLEVEL, point5 * detailLevel, point6 * detailLevel, getDensityAtPoint(densities, point5.x * detailLevel, point5.y * detailLevel, point5.z * detailLevel), getDensityAtPoint(densities, point6.x * detailLevel, point6.y * detailLevel, point6.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 64) {
		vertList[6] = VertexInterp(ISOLEVEL, point6 * detailLevel, point7 * detailLevel, getDensityAtPoint(densities, point6.x * detailLevel, point6.y * detailLevel, point6.z * detailLevel), getDensityAtPoint(densities, point7.x * detailLevel, point7.y * detailLevel, point7.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 128) {
		vertList[7] = VertexInterp(ISOLEVEL, point7 * detailLevel, point4 * detailLevel, getDensityAtPoint(densities, point7.x * detailLevel, point7.y * detailLevel, point7.z * detailLevel), getDensityAtPoint(densities, point4.x * detailLevel, point4.y * detailLevel, point4.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 256) {
		vertList[8] = VertexInterp(ISOLEVEL, point0 * detailLevel, point4 * detailLevel, getDensityAtPoint(densities, point0.x * detailLevel, point0.y * detailLevel, point0.z * detailLevel), getDensityAtPoint(densities, point4.x * detailLevel, point4.y * detailLevel, point4.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 512) {
		vertList[9] = VertexInterp(ISOLEVEL, point1 * detailLevel, point5 * detailLevel, getDensityAtPoint(densities, point1.x * detailLevel, point1.y * detailLevel, point1.z * detailLevel), getDensityAtPoint(densities, point5.x * detailLevel, point5.y * detailLevel, point5.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 1024) {
		vertList[10] = VertexInterp(ISOLEVEL, point2 * detailLevel, point6 * detailLevel, getDensityAtPoint(densities, point2.x * detailLevel, point2.y * detailLevel, point2.z * detailLevel), getDensityAtPoint(densities, point6.x * detailLevel, point6.y * detailLevel, point6.z * detailLevel));
	}
	if (edgeTable[cubeIndex] & 2048) {
		vertList[11] = VertexInterp(ISOLEVEL, point3 * detailLevel, point7 * detailLevel, getDensityAtPoint(densities, point3.x * detailLevel, point3.y * detailLevel, point3.z * detailLevel), getDensityAtPoint(densities, point7.x * detailLevel, point7.y * detailLevel, point7.z * detailLevel));
	}

	for (unsigned int i = 0; triTable[cubeIndex][i] != -1; i += 3) {
		Vector3 v1 = vertList[triTable[cubeIndex][i]];
		Vector3 v2 = vertList[triTable[cubeIndex][i + 1]];
		Vector3 v3 = vertList[triTable[cubeIndex][i + 2]];
		
		addVertex(v1, v2, v3, vertices);
	
	}

	return vertices;
	
}