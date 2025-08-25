#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() {
	shaderProgram = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(shaderProgram);
}

void ShaderProgram::attachShader(Shader& shader) {
	glAttachShader(shaderProgram, shader.shader);
}

void ShaderProgram::link() {
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
	}
}

void ShaderProgram::use() {
	glUseProgram(shaderProgram);
}