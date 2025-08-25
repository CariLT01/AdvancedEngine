#include "VAO.h"
#include <iostream>
#include <stdexcept>

VAO::VAO(std::vector<VertexAttribute> vertexAttributes, VBO* vbo) {
	if (vertexAttributes.empty()) {
		throw std::runtime_error("Vertex attributes cannot be empty");
	}
	
	this->vbo = vbo;
	
	glGenVertexArrays(1, &vao);
	bind();
	vbo->bind();
	
	unsigned int stride = 0;

	for (const auto& attribute : vertexAttributes) {
		stride += attribute.sizeInBytes;
	}

	unsigned int currentOffset = 0;
	unsigned int index = 0;

	this->numberOfValuesPerVertex = 0;

	std::cout << "--- Setting vertex attributes ---" << std::endl;

	for (const auto& attribute : vertexAttributes) {
		glVertexAttribPointer(
			index,
			attribute.size,          // number of components (1, 2, 3, 4)
			attribute.type,          // GL_FLOAT, etc.
			attribute.normalized,    // GL_TRUE or GL_FALSE
			stride,                  // stride between consecutive vertices
			reinterpret_cast<void*>(currentOffset) // offset in the buffer
		);

		std::cout << "Attribute: " << index
			<< ", Size: " << attribute.size
			<< ", Type: " << attribute.type
			<< ", Normalized: " << (attribute.normalized ? "true" : "false")
			<< ", Offset: " << currentOffset
			<< ", Stride: " << stride << std::endl;

		glEnableVertexAttribArray(index);

		index++;
		currentOffset += attribute.sizeInBytes;
		this->numberOfValuesPerVertex += attribute.size;
	}

	std::cout << "--- Finished setting vertex attributes ---" << std::endl;
	std::cout << "Number of values per vertex: " << this->numberOfValuesPerVertex << std::endl;

	unbind();
}

void VAO::bind() {
	glBindVertexArray(vao);
}

void VAO::unbind() {
	glBindVertexArray(0);
}