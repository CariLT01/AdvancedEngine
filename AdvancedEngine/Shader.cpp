#include "Shader.h"
#include <iostream>

Shader::Shader(const char* source, GLenum type) {
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
}

Shader::~Shader() {
	glDeleteShader(shader);
}

void Shader::compile() {
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "Shader compilation error: " << infoLog << std::endl;
		throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
	}
}