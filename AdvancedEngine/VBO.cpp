#include "VBO.h"
#include <glad/gl.h>


VBO::VBO(std::vector<float> vertices, GLenum usage) {

	size = vertices.size();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), usage);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

VBO::~VBO() {
	glDeleteBuffers(1, &vbo);
}