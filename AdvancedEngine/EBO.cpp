#include "EBO.h"
#include <iostream>


EBO::EBO(std::vector<unsigned int> indices, GLenum usage) {
	glGenBuffers(1, &ebo);
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usage);

	std::cout << "EBO created with ID: " << ebo << std::endl;
	std::cout << "EBO size (in bytes): " << indices.size() * sizeof(unsigned int) << std::endl;

	numberOfElements = indices.size();
	
	std::cout << "Number of elements: " << numberOfElements << std::endl;
}
void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}
void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
EBO::~EBO() {
	glDeleteBuffers(1, &ebo);
}