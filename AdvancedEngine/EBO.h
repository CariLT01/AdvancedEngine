#pragma once

#include <vector>
#include <glad/gl.h>


class EBO {
public:
	EBO(std::vector<unsigned int> indices, GLenum usage);
	~EBO();

	void bind();
	void unbind();

	GLuint ebo;

	unsigned int numberOfElements;

};