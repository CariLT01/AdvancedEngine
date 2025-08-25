#pragma once

#include "VAO.h"
#include "ShaderProgram.h"

class Material {
public:
	Material(const char* vertexShaderSource, const char* fragmentShaderSource, std::vector<VertexAttribute> vertexAttributes);

	ShaderProgram* shaderProgram;

	void use();

	std::vector<VertexAttribute> vertexAttributes;
};