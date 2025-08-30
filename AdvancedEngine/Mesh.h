#pragma once

#include <vector>
#include "Material.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
/*
A Mesh represents a 3D object in the scene. It contains geometry data (vertices and indices)
It does not transform the object in the world; that is handled by a WorldObject.
IT does not set shader uniforms.

It is responsible for setting up the VAO, VBO, and EBO, and issuing the draw call.
*/
class Mesh {
public:
	Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, Material* material);
	~Mesh();

	Material* material;

	VAO* vao;
	VBO* vbo;
	EBO* ebo;

	void prepareUniforms(); // This binds the shader program, allowing you to set uniforms before calling .render()
	void render(); // Binds VBO, VAO, and EBO and issues the draw call
};