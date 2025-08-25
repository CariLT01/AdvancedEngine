#pragma once

#include <vector>
#include "Material.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"

class Mesh {
public:
	Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, Material* material);

	Material* material;

	VAO* vao;
	VBO* vbo;
	EBO* ebo;

	void render();
};