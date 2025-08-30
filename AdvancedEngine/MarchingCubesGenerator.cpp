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

	std::cout << "Densities has a length of: " << densities.size() << std::endl;


	for (unsigned int y = 0; y < ((CHUNK_SIZE) / detailLevel); y++) {
		for (unsigned int x = 0; x < ((CHUNK_SIZE) / detailLevel); x++) {
			for (unsigned int z = 0; z < ((CHUNK_SIZE) / detailLevel); z++) {

				

				const std::vector<float> cellVertices = buildCell(x, y, z, densities, materials, detailLevel);

				for (const float vertex : cellVertices) {
					notEmptyCount++;
					vertices.push_back(vertex);
				}

			}
		}
	}
	//std::cout << "Marching Cubes generated " << notEmptyCount / 6 << " triangles." << std::endl;

	return vertices;
}

float MarchingCubeGenerator::getDensityAtPoint(std::vector<float>& densities, const unsigned int& x, const unsigned int& y, const unsigned int& z) {
#ifdef _DEBUG
	const unsigned int index = z + x * (CHUNK_SIZE + 1) + y * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1);
	if (index >= densities.size()) {
		std::cerr << "Index out of bounds in getDensityAtPoint: " << index << " (max: " << densities.size() << ")" << std::endl;
		std::cerr << "X:" << x << "Y: " << y << "Z: " << z << std::endl;
		return 0.0f;
	}
	return densities[index];
#endif

#ifndef _DEBUG
	return densities[z + x * (CHUNK_SIZE + 1) + y * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1)];
#endif
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

void addVertex(const Vector3& a, const Vector3& b, const Vector3& c, std::vector<float>& vertices, float material) {
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
	vertices.push_back(material);

	vertices.push_back(b.x);
	vertices.push_back(b.y);
	vertices.push_back(b.z);
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);
	vertices.push_back(material);

	vertices.push_back(c.x);
	vertices.push_back(c.y);
	vertices.push_back(c.z);
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);
	vertices.push_back(material);


}



std::vector<float> MarchingCubeGenerator::buildCell(const unsigned int& localX, const unsigned int& localY, const unsigned int& localZ, std::vector<float>& densities, std::vector<unsigned int>& materials, const unsigned int& detailLevel) {
	std::vector<float> vertices;
	
	const Vector3 point0 = { static_cast<float>(localX), static_cast<float>(localY), static_cast<float>(localZ) };
	const Vector3 point1 = { static_cast<float>(localX) + 1, static_cast<float>(localY), static_cast<float>(localZ) };
	const Vector3 point2 = { static_cast<float>(localX) + 1, static_cast<float>(localY), static_cast<float>(localZ) + 1 };
	const Vector3 point3 = { static_cast<float>(localX), static_cast<float>(localY), static_cast<float>(localZ) + 1 };
	const Vector3 point4 = { static_cast<float>(localX), static_cast<float>(localY) + 1, static_cast<float>(localZ) };
	const Vector3 point5 = { static_cast<float>(localX) + 1, static_cast<float>(localY) + 1, static_cast<float>(localZ) };
	const Vector3 point6 = { static_cast<float>(localX) + 1, static_cast<float>(localY) + 1, static_cast<float>(localZ) + 1 };
	const Vector3 point7 = { static_cast<float>(localX), static_cast<float>(localY) + 1, static_cast<float>(localZ) + 1 };

	Vector3 vertList[12] = { {0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};

	constexpr float ISOLEVEL = 0.5f;

	unsigned int cubeIndex = 0;

	if (getDensityAtPoint(densities, static_cast<unsigned int>(point0.x) * detailLevel, static_cast<unsigned int>(point0.y) * detailLevel, static_cast<unsigned int>(point0.z) * detailLevel) < ISOLEVEL) cubeIndex |= 1;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point1.x) * detailLevel, static_cast<unsigned int>(point1.y) * detailLevel, static_cast<unsigned int>(point1.z) * detailLevel) < ISOLEVEL) cubeIndex |= 2;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point2.x) * detailLevel, static_cast<unsigned int>(point2.y) * detailLevel, static_cast<unsigned int>(point2.z) * detailLevel) < ISOLEVEL) cubeIndex |= 4;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point3.x) * detailLevel, static_cast<unsigned int>(point3.y) * detailLevel, static_cast<unsigned int>(point3.z) * detailLevel) < ISOLEVEL) cubeIndex |= 8;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point4.x) * detailLevel, static_cast<unsigned int>(point4.y) * detailLevel, static_cast<unsigned int>(point4.z) * detailLevel) < ISOLEVEL) cubeIndex |= 16;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point5.x) * detailLevel, static_cast<unsigned int>(point5.y) * detailLevel, static_cast<unsigned int>(point5.z) * detailLevel) < ISOLEVEL) cubeIndex |= 32;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point6.x) * detailLevel, static_cast<unsigned int>(point6.y) * detailLevel, static_cast<unsigned int>(point6.z) * detailLevel) < ISOLEVEL) cubeIndex |= 64;
	if (getDensityAtPoint(densities, static_cast<unsigned int>(point7.x) * detailLevel, static_cast<unsigned int>(point7.y) * detailLevel, static_cast<unsigned int>(point7.z) * detailLevel) < ISOLEVEL) cubeIndex |= 128;

	if (edgeTable[cubeIndex] == 0) {
		return {};
	}

	if (edgeTable[cubeIndex] & 1) {
		vertList[0] = VertexInterp(ISOLEVEL, point0 * static_cast<float>(detailLevel), point1 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point0.x) * detailLevel, static_cast<unsigned int>(point0.y) * detailLevel, static_cast<unsigned int>(point0.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point1.x) * detailLevel, static_cast<unsigned int>(point1.y) * detailLevel, static_cast<unsigned int>(point1.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 2) {
		vertList[1] = VertexInterp(ISOLEVEL, point1 * static_cast<float>(detailLevel), point2 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point1.x) * detailLevel, static_cast<unsigned int>(point1.y) * detailLevel, static_cast<unsigned int>(point1.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point2.x) * detailLevel, static_cast<unsigned int>(point2.y) * detailLevel, static_cast<unsigned int>(point2.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 4) {
		vertList[2] = VertexInterp(ISOLEVEL, point2 * static_cast<float>(detailLevel), point3 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point2.x) * detailLevel, static_cast<unsigned int>(point2.y) * detailLevel, static_cast<unsigned int>(point2.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point3.x) * detailLevel, static_cast<unsigned int>(point3.y) * detailLevel, static_cast<unsigned int>(point3.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 8) {
		vertList[3] = VertexInterp(ISOLEVEL, point3 * static_cast<float>(detailLevel), point0 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point3.x) * detailLevel, static_cast<unsigned int>(point3.y) * detailLevel, static_cast<unsigned int>(point3.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point0.x) * detailLevel, static_cast<unsigned int>(point0.y) * detailLevel, static_cast<unsigned int>(point0.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 16) {
		vertList[4] = VertexInterp(ISOLEVEL, point4 * static_cast<float>(detailLevel), point5 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point4.x) * detailLevel, static_cast<unsigned int>(point4.y) * detailLevel, static_cast<unsigned int>(point4.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point5.x) * detailLevel, static_cast<unsigned int>(point5.y) * detailLevel, static_cast<unsigned int>(point5.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 32) {
		vertList[5] = VertexInterp(ISOLEVEL, point5 * static_cast<float>(detailLevel), point6 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point5.x) * detailLevel, static_cast<unsigned int>(point5.y) * detailLevel, static_cast<unsigned int>(point5.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point6.x) * detailLevel, static_cast<unsigned int>(point6.y) * detailLevel, static_cast<unsigned int>(point6.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 64) {
		vertList[6] = VertexInterp(ISOLEVEL, point6 * static_cast<float>(detailLevel), point7 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point6.x) * detailLevel, static_cast<unsigned int>(point6.y) * detailLevel, static_cast<unsigned int>(point6.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point7.x) * detailLevel, static_cast<unsigned int>(point7.y) * detailLevel, static_cast<unsigned int>(point7.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 128) {
		vertList[7] = VertexInterp(ISOLEVEL, point7 * static_cast<float>(detailLevel), point4 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point7.x) * detailLevel, static_cast<unsigned int>(point7.y) * detailLevel, static_cast<unsigned int>(point7.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point4.x) * detailLevel, static_cast<unsigned int>(point4.y) * detailLevel, static_cast<unsigned int>(point4.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 256) {
		vertList[8] = VertexInterp(ISOLEVEL, point0 * static_cast<float>(detailLevel), point4 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point0.x) * detailLevel, static_cast<unsigned int>(point0.y) * detailLevel, static_cast<unsigned int>(point0.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point4.x) * detailLevel, static_cast<unsigned int>(point4.y) * detailLevel, static_cast<unsigned int>(point4.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 512) {
		vertList[9] = VertexInterp(ISOLEVEL, point1 * static_cast<float>(detailLevel), point5 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point1.x) * detailLevel, static_cast<unsigned int>(point1.y) * detailLevel, static_cast<unsigned int>(point1.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point5.x) * detailLevel, static_cast<unsigned int>(point5.y) * detailLevel, static_cast<unsigned int>(point5.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 1024) {
		vertList[10] = VertexInterp(ISOLEVEL, point2 * static_cast<float>(detailLevel), point6 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point2.x) * detailLevel, static_cast<unsigned int>(point2.y) * detailLevel, static_cast<unsigned int>(point2.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point6.x) * detailLevel, static_cast<unsigned int>(point6.y) * detailLevel, static_cast<unsigned int>(point6.z) * detailLevel));
	}
	if (edgeTable[cubeIndex] & 2048) {
		vertList[11] = VertexInterp(ISOLEVEL, point3 * static_cast<float>(detailLevel), point7 * static_cast<float>(detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point3.x) * detailLevel, static_cast<unsigned int>(point3.y) * detailLevel, static_cast<unsigned int>(point3.z) * detailLevel), getDensityAtPoint(densities, static_cast<unsigned int>(point7.x) * detailLevel, static_cast<unsigned int>(point7.y) * detailLevel, static_cast<unsigned int>(point7.z) * detailLevel));
	}

	for (unsigned int i = 0; triTable[cubeIndex][i] != -1; i += 3) {
		Vector3 v1 = vertList[triTable[cubeIndex][i]];
		Vector3 v2 = vertList[triTable[cubeIndex][i + 1]];
		Vector3 v3 = vertList[triTable[cubeIndex][i + 2]];
		
		const unsigned int index = point0.z + point0.x * (CHUNK_SIZE + 1) + point0.y * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1);



		addVertex(v1, v3, v2, vertices, static_cast<float>(materials[index]));
	
	}

	//std::cout << "generated " << vertices.size() / 6 << " vertices for cell at " << static_cast<float>(localX) << ", " << static_cast<float>(localY) << ", " << static_cast<float>(localZ) << std::endl;

	return vertices;
	
}