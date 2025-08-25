#pragma once

#include "Shader.h"

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void attachShader(Shader& shader);
	void link();
	void use();

	GLint shaderProgram;
};