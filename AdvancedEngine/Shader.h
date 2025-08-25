#pragma once

#include <glad/gl.h>
#include <stdexcept>

class Shader {
public:
	Shader(const char* source, GLenum shaderType);
	~Shader();

	void compile();

	 GLint shader;
};