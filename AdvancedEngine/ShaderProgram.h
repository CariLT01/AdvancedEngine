#pragma once

#include "Shader.h"

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void attachShader(Shader& shader);
	void link();
	void use();
	int getUniformLocation(const char* name);

	GLint shaderProgram;
};