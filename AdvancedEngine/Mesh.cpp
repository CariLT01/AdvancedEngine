#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


Mesh::Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, Material* material) {
	this->material = material;
	vbo = new VBO(vertices, GL_STATIC_DRAW);
	std::vector<VertexAttribute> vertexAttributes = material->vertexAttributes;
	vao = new VAO(vertexAttributes, vbo);
	ebo = new EBO(indices, GL_STATIC_DRAW);
}

Mesh::~Mesh() {
	delete vbo;
	delete vao;
	delete ebo;
}

void Mesh::prepareUniforms() {

	material->use();

}


void Mesh::render() {
	
	vao->bind();
	ebo->bind();
	glDrawElements(GL_TRIANGLES, ebo->numberOfElements, GL_UNSIGNED_INT, 0);
	vao->unbind();
}