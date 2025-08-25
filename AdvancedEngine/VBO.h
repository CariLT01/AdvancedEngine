#pragma once

#include <vector>
#include <glad/gl.h>

class VBO {
public:
	VBO(std::vector<float> vertices, GLenum usage);
	~VBO();

	GLuint vbo;
	unsigned int size;

	void bind();
};