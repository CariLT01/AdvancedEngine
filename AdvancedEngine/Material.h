#pragma once

#include "VAO.h"
#include "ShaderProgram.h"

class Material {
public:
	Material(const char* vertexShaderSource, const char* fragmentShaderSource, std::vector<VertexAttribute> vertexAttributes);

	ShaderProgram* shaderProgram;

	virtual void use();
	int getUniformLocation(const char* name);



	std::vector<VertexAttribute> vertexAttributes;
};