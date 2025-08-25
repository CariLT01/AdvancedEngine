#pragma once

#include <glad/gl.h>
#include "VBO.h"
#include <vector>


struct VertexAttribute {
	unsigned int sizeInBytes;
	unsigned int size;
	GLenum type;
	bool normalized;
};

class VAO {
public:
	VAO(std::vector<VertexAttribute> vertexAttributes, VBO* vbo);
	~VAO();

	void bind();
	void unbind();

	GLuint vao;

	unsigned int numberOfValuesPerVertex;

	VBO* vbo;
};