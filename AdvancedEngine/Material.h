#pragma once

#include "VAO.h"
#include "ShaderProgram.h"
#include "Camera.h"

class Material {
public:
	Material(const char* vertexShaderSource, const char* fragmentShaderSource, std::vector<VertexAttribute> vertexAttributes);

	ShaderProgram* shaderProgram;

	virtual void use();
	virtual void use2(Camera* camera);
	int getUniformLocation(const char* name);
	void setMatrices(Camera* camera);



	std::vector<VertexAttribute> vertexAttributes;
};