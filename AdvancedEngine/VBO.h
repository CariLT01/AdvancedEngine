#pragma once

#include <glad/gl.h>
#include <vector>

class VBO {
public:
	VBO(std::vector<float> vertices, GLenum usage);
	~VBO();

	GLuint vbo;
	unsigned int size;

	void bind();
};